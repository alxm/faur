/*
    Copyright 2010 Alex Margarit <alex@alxm.org> and:

    - Wiz framebuffer direction set code by Orkie
    - Wiz/Caanoo timer code by notaz (https://github.com/notaz/libpicofe)

    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_wiz.v.h"
#include <faur.v.h>

#if F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#if F_CONFIG_SYSTEM_WIZ
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
        F__FATAL("Could not set up Pollux timer");
    }

    if(TIMER_REG(0x08) & 8) { // Timer in use
        timer_clean();
    }

    div2 = (div2 + 3) & 3;

    TIMER_REG(0x44) = ((div - 1) << 4) | 2; // Using PLL1
    TIMER_REG(0x40) = 0x0c; // Clocks on
    TIMER_REG(0x08) = 0x68 | div2; // Run timer, clear irq, latch value
}

void f_platform_wiz__init(void)
{
    #if F_CONFIG_SYSTEM_WIZ
        if(f_path_exists("./mmuhack.ko", F_PATH_FILE)) {
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

void f_platform_wiz__uninit(void)
{
    timer_clean();
    close(g_memfd);

    #if F_CONFIG_SYSTEM_WIZ
        if(g_mmuHackOn) {
            system("/sbin/rmmod mmuhack");
        }
    #endif
}

uint32_t f_platform_api__timeMsGet(void)
{
    unsigned div = TIMER_REG(0x08) & 3;
    TIMER_REG(0x08) = 0x48 | div; // Run timer, latch value

    return TIMER_REG(0) / 1000;
}

void f_platform_api__timeMsWait(uint32_t Ms)
{
    f_time_msSpin(Ms);
}

#if F_CONFIG_SYSTEM_WIZ
void f_platform_wiz__portraitModeSet(void)
{
    // Set Wiz screen to portrait mode to avoid diagonal tearing
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
#endif // F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
