/*
    Copyright 2010, 2016 Alex Margarit and:

    - GP2X clock speed from the GP2X Wiki
    - GP2X mmuhack by Squidge and NK
    - GP2X ram timings by JyCet
    - Wiz framebuffer direction by Orkie
    - Wiz/Caanoo timer by notaz (https://github.com/notaz/libpicofe)

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_hw.v.h"

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static int g_mmuHackOn = 0;
#endif

#if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    #define TIMER_BASE3  0x1980
    #define TIMER_REG(x) g_memregs[(TIMER_BASE3 + x) >> 2]

    static int g_memfd;
    static volatile uint32_t* g_memregs;

    static int decode_pll(unsigned reg)
    {
        long long v;
        int p, m, s;

        p = (reg >> 18) & 0x3f;
        m = (reg >> 8) & 0x3ff;
        s = reg & 0xff;

        if(p == 0) {
            p = 1;
        }

        v = 27000000; // master clock
        v = v * m / (p << s);

        return v;
    }

    static void timer_clean(void)
    {
        TIMER_REG(0x40) = 0x0c; // Be sure clocks are on
        TIMER_REG(0x08) = 0x23; // Stop the timer, clear irq in case it's pending
        TIMER_REG(0x00) = 0; // Clear counter
        TIMER_REG(0x40) = 0; // Clocks off
        TIMER_REG(0x44) = 0; // Dividers back to default
    }

    static void timer_init(void)
    {
        unsigned rate = decode_pll(g_memregs[0xf008 >> 2]);
        unsigned div = (rate + 500000) / 1000000;
        unsigned div2 = 0;

        while(div > 256) {
            div /= 2;
            div2++;
        }

        if(div < 1 || div > 256 || div2 >= 4) {
            a_out__fatal("Could not set up timer");
        }

        if(TIMER_REG(0x08) & 8) { // Timer in use
            timer_clean();
        }

        div2 = (div2 + 3) & 3;

        TIMER_REG(0x44) = ((div - 1) << 4) | 2; // Using PLL1
        TIMER_REG(0x40) = 0x0c; // Clocks on
        TIMER_REG(0x08) = 0x68 | div2; // Run timer, clear irq, latch value
    }
#endif

#if A_PLATFORM_PANDORA
    static struct {
        char* nub0Mode;
        char* nub1Mode;
    } g_pandora;

    static void pandora_setNubModes(const char* Nub0, const char* Nub1)
    {
        char cmd[128];
        int r = snprintf(cmd,
                         sizeof(cmd),
                         "/usr/pandora/scripts/op_nubchange.sh %s %s",
                         Nub0,
                         Nub1);

        if(r < 0 || r >= (int)sizeof(cmd)) {
            a_out__error("pandora_setNubModes snprintf failed");
            return;
        }

        if(system(cmd) < 0) {
            a_out__error("op_nubchange.sh failed");
            return;
        }

        a_out__message("Set nub modes to %s, %s", Nub0, Nub1);
    }

    static void pandora_setScreenFilter(const char* Value)
    {
        char cmd[128];
        int r = snprintf(cmd,
                         sizeof(cmd),
                         "sudo -n /usr/pandora/scripts/op_videofir.sh %s",
                         Value);

        if(r < 0 || r >= (int)sizeof(cmd)) {
            a_out__error("pandora_setScreenFilter snprintf failed");
            return;
        }

        if(system(cmd) < 0) {
            a_out__error("op_videofir.sh failed");
            return;
        }

        a_out__message("Set screen filter to %s", Value);
    }
#endif

#if A_PLATFORM_GP2X
    static void setCpuSpeed(unsigned MHz)
    {
        unsigned mhz = MHz * 1000000;
        unsigned freq = 7372800;

        int memfd = open("/dev/mem", O_RDWR);
        volatile uint32_t* memregs32 = mmap(0,
                                            0x10000,
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED,
                                            memfd,
                                            0xc0000000);
        volatile uint16_t* memregs16 = (uint16_t*)memregs32;

        unsigned pdiv = 3;
        unsigned mdiv = (mhz * pdiv) / freq;

        pdiv = ((pdiv - 2) << 2) & 0xfc;
        mdiv = ((mdiv - 8) << 8) & 0xff00;

        unsigned v = pdiv | mdiv;
        unsigned l = memregs32[0x808 >> 2]; // Get interupt flags

        memregs32[0x808 >> 2] = 0xFF8FFFE7; // Turn off interrupts
        memregs16[0x910 >> 1] = (uint16_t)v; // Set frequency

        while(memregs16[0x0902 >> 1] & 1) {
            // Wait for the frequency to be adjusted
            continue;
        }

        memregs32[0x808 >> 2] = l; // Turn on interrupts

        close(memfd);
    }

    static void setRamTimings(unsigned tRC, unsigned tRAS, unsigned tWR, unsigned tMRD, unsigned tRFC, unsigned tRP, unsigned tRCD)
    {
        int memfd = open("/dev/mem", O_RDWR);
        volatile uint16_t* memregs16 = mmap(0,
                                            0x10000,
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED,
                                            memfd,
                                            0xc0000000);

        tRC -= 1;
        tRAS -= 1;
        tWR -= 1;
        tMRD -= 1;
        tRFC -= 1;
        tRP -= 1;
        tRCD -= 1;

        memregs16[0x3802 >> 1] = (uint16_t)(((tMRD & 0xF) << 12) |
                                            ((tRFC & 0xF) << 8)  |
                                            ((tRP  & 0xF) << 4)  |
                                             (tRCD & 0xF));

        memregs16[0x3804 >> 1] = (uint16_t)(((tRC  & 0xF) << 8) |
                                            ((tRAS & 0xF) << 4) |
                                             (tWR  & 0xF));

        close(memfd);
    }
#endif

void a_hw__init_preSDL(void)
{
    #if A_PLATFORM_PANDORA
        g_pandora.nub0Mode = NULL;
        g_pandora.nub1Mode = NULL;

        AFile* nub0 = a_file_open("/proc/pandora/nub0/mode", "r");
        AFile* nub1 = a_file_open("/proc/pandora/nub1/mode", "r");

        if(nub0 && nub1 && a_file_readLine(nub0) && a_file_readLine(nub1)) {
            g_pandora.nub0Mode = a_str_dup(a_file_getLine(nub0));
            a_out__message("Current nub0Mode is '%s'", g_pandora.nub0Mode);

            g_pandora.nub1Mode = a_str_dup(a_file_getLine(nub1));
            a_out__message("Current nub1Mode is '%s'", g_pandora.nub1Mode);
        }

        if(nub0) {
            a_file_close(nub0);
        }

        if(nub1) {
            a_file_close(nub1);
        }

        // Gamepad mode
        pandora_setNubModes("absolute", "absolute");

        // Crisp pixels, no blur filter
        pandora_setScreenFilter("none");
    #endif
}

void a_hw__init_postSDL(void)
{
    #if A_PLATFORM_GP2X
        if(a_file_exists("./mmuhack.o")) {
            system("/sbin/rmmod mmuhack");
            system("/sbin/insmod mmuhack.o");

            int mmufd = open("/dev/mmuhack", O_RDWR);

            if(mmufd >= 0) {
                close(mmufd);
                g_mmuHackOn = 1;
            }
        }

        if(a_settings_getUnsigned("app.mhz") > 0) {
            setCpuSpeed(a_settings_getUnsigned("app.mhz"));
        }

        setRamTimings(6, 4, 1, 1, 1, 2, 2);
    #elif A_PLATFORM_WIZ
        if(a_file_exists("./mmuhack.ko")) {
            system("/sbin/rmmod mmuhack");
            system("/sbin/insmod mmuhack.ko");

            int mmufd = open("/dev/mmuhack", O_RDWR);

            if(mmufd >= 0) {
                close(mmufd);
                g_mmuHackOn = 1;
            }
        }
    #endif

    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        g_memfd = open("/dev/mem", O_RDWR);
        g_memregs = mmap(0,
                         0x20000,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         g_memfd,
                         0xc0000000);

        timer_init();
    #endif
}

void a_hw__uninit(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        timer_clean();
        close(g_memfd);
    #endif

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(g_mmuHackOn) {
            system("/sbin/rmmod mmuhack");
        }
    #endif

    #if A_PLATFORM_GP2X
        setRamTimings(8, 16, 3, 8, 8, 8, 8);
    #endif

    #if A_PLATFORM_PANDORA
        if(g_pandora.nub0Mode && g_pandora.nub1Mode) {
            pandora_setNubModes(g_pandora.nub0Mode, g_pandora.nub1Mode);
        }

        free(g_pandora.nub0Mode);
        free(g_pandora.nub1Mode);

        pandora_setScreenFilter("default");
    #endif
}

#if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    uint32_t a_hw__getMs(void)
    {
        unsigned div = TIMER_REG(0x08) & 3;
        TIMER_REG(0x08) = 0x48 | div; // Run timer, latch value

        return TIMER_REG(0) / 1000;
    }

    void a_hw__setWizPortraitMode(void)
    {
        #define FBIO_MAGIC 'D'
        #define FBIO_LCD_CHANGE_CONTROL _IOW(FBIO_MAGIC, 90, unsigned[2])
        #define LCD_DIRECTION_ON_CMD 5 // 320x240
        #define LCD_DIRECTION_OFF_CMD 6 // 240x320

        unsigned send[2];
        int fb_fd = open("/dev/fb0", O_RDWR);
        send[0] = LCD_DIRECTION_OFF_CMD;
        ioctl(fb_fd, FBIO_LCD_CHANGE_CONTROL, &send);
        close(fb_fd);
    }
#endif
