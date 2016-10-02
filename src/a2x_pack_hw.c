/*
    Copyright 2010, 2016 Alex Margarit

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

/*
    Many thanks to:

    - A wiki poster for the GP2X clock speed setter
    - Squidge and NK for the GP2X mmuhack
    - JyCet for the GP2X ram timings code
    - Notaz for the Wiz mmuhack and accurate timer code
    - Orkie for the Wiz framebuffer direction code
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
#endif

#if A_PLATFORM_GP2X
    static void setCpuSpeed(int MHz)
    {
        int mhz = MHz * 1000000;
        int freq = 7372800;

        int memfd = open("/dev/mem", O_RDWR);
        volatile uint32_t* memregs32 = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0xc0000000);
        volatile uint16_t* memregs16 = (uint16_t*)memregs32;

        unsigned int pdiv = 3;
        unsigned int mdiv = (mhz * pdiv) / freq;

        pdiv = ((pdiv - 2) << 2) & 0xfc;
        mdiv = ((mdiv - 8) << 8) & 0xff00;

        unsigned int v = pdiv | mdiv;

        unsigned int l = memregs32[0x808 >> 2]; // Get interupt flags
        memregs32[0x808 >> 2] = 0xFF8FFFE7; // Turn off interrupts
        memregs16[0x910 >> 1] = v; // Set frequentie
        while(memregs16[0x0902 >> 1] & 1) continue; // Wait for the frequentie to be ajused
        memregs32[0x808 >> 2] = l; // Turn on interrupts

        close(memfd);
    }

    static void setRamTimings(int tRC, int tRAS, int tWR, int tMRD, int tRFC, int tRP, int tRCD)
    {
        int memfd = open("/dev/mem", O_RDWR);
        volatile uint32_t* memregs32 = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0xc0000000);
        volatile uint16_t* memregs16 = (uint16_t*)memregs32;

        tRC -= 1; tRAS -= 1; tWR -= 1; tMRD -= 1; tRFC -= 1; tRP -= 1; tRCD -= 1;
        memregs16[0x3802>>1] = ((tMRD & 0xF) << 12) | ((tRFC & 0xF) << 8) | ((tRP & 0xF) << 4) | (tRCD & 0xF);
        memregs16[0x3804>>1] = ((tRC & 0xF) << 8) | ((tRAS & 0xF) << 4) | (tWR & 0xF);

        close(memfd);
    }
#endif

void a_hw__init(void)
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

        if(a_settings_getInt("app.mhz") > 0) {
            setCpuSpeed(a_settings_getInt("app.mhz"));
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
        g_memregs = mmap(0, 0x20000, PROT_READ | PROT_WRITE, MAP_SHARED, g_memfd, 0xc0000000);

        TIMER_REG(0x44) = 0x922;
        TIMER_REG(0x40) = 0x0c;
        TIMER_REG(0x08) = 0x6b;
    #endif
}

void a_hw__uninit(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        TIMER_REG(0x40) = 0x0c;
        TIMER_REG(0x08) = 0x23;
        TIMER_REG(0x00) = 0;
        TIMER_REG(0x40) = 0;
        TIMER_REG(0x44) = 0;

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
}

#if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    uint32_t a_hw__getMs(void)
    {
        TIMER_REG(0x08) = 0x4b; // run timer, latch value
        return TIMER_REG(0) / 1000;
    }

    void a_hw__setWizPortraitMode(void)
    {
        #define FBIO_MAGIC 'D'
        #define FBIO_LCD_CHANGE_CONTROL _IOW(FBIO_MAGIC, 90, unsigned int[2])
        #define LCD_DIRECTION_ON_CMD 5 // 320x240
        #define LCD_DIRECTION_OFF_CMD 6 // 240x320

        unsigned int send[2];
        int fb_fd = open("/dev/fb0", O_RDWR);
        send[0] = LCD_DIRECTION_OFF_CMD;
        ioctl(fb_fd, FBIO_LCD_CHANGE_CONTROL, &send);
        close(fb_fd);
    }
#endif
