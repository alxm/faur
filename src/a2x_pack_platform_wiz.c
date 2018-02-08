/*
    Copyright 2010, 2016, 2017 Alex Margarit and:

    - Wiz framebuffer direction set code by Orkie
    - Wiz/Caanoo timer code by notaz (https://github.com/notaz/libpicofe)

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

#include "a2x_system_includes.h"

#if A_PLATFORM_SYSTEM_WIZ || A_PLATFORM_SYSTEM_CAANOO
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "a2x_pack_file.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform_wiz.v.h"
#include "a2x_pack_settings.v.h"

#if A_PLATFORM_SYSTEM_WIZ
    static bool g_mmuHackOn = false;
#endif

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

void a_platform_wiz__init(void)
{
    #if A_PLATFORM_SYSTEM_WIZ
        if(a_file_exists("./mmuhack.ko")) {
            system("/sbin/rmmod mmuhack");
            system("/sbin/insmod mmuhack.ko");

            int mmufd = open("/dev/mmuhack", O_RDWR);

            if(mmufd >= 0) {
                close(mmufd);
                g_mmuHackOn = true;
            }
        }
    #endif

    g_memfd = open("/dev/mem", O_RDWR);
    g_memregs = mmap(0,
                     0x20000,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     g_memfd,
                     0xc0000000);

    timer_init();
}

void a_platform_wiz__uninit(void)
{
    timer_clean();
    close(g_memfd);

    #if A_PLATFORM_SYSTEM_WIZ
        if(g_mmuHackOn) {
            system("/sbin/rmmod mmuhack");
        }
    #endif
}

uint32_t a_platform__getMs(void)
{
    unsigned div = TIMER_REG(0x08) & 3;
    TIMER_REG(0x08) = 0x48 | div; // Run timer, latch value

    return TIMER_REG(0) / 1000;
}

#if A_PLATFORM_SYSTEM_WIZ
void a_platform_wiz__setScreenPortraitMode(void)
{
    // Set Wiz screen to portrait mode to avoid diagonal tearing
    if(!a_settings_getBool("video.fixWizTearing")) {
        return;
    }

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

void a_platform__getNativeResolution(int* Width, int* Height)
{
    *Width = 320;
    *Height = 240;
}
#endif // A_PLATFORM_SYSTEM_WIZ || A_PLATFORM_SYSTEM_CAANOO
