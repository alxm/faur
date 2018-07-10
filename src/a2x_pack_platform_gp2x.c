/*
    Copyright 2010, 2016-2018 Alex Margarit and:

    - GP2X clock speed from the GP2X Wiki
    - GP2X mmuhack by Squidge and NK
    - GP2X ram timings by JyCet

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

#if A_PLATFORM_SYSTEM_GP2X
#include "a2x_pack_platform_gp2x.v.h"

#include <fcntl.h>
#include <sys/mman.h>

#include "a2x_pack_file.v.h"
#include "a2x_pack_settings.v.h"

static int g_mmuHackOn = 0;

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

void a_platform_gp2x__init(void)
{
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
}

void a_platform_gp2x__uninit(void)
{
    if(g_mmuHackOn) {
        system("/sbin/rmmod mmuhack");
    }

    setRamTimings(8, 16, 3, 8, 8, 8, 8);
}

void a_platform__screenGetNativeResolution(int* Width, int* Height)
{
    *Width = 320;
    *Height = 240;
}
#endif // A_PLATFORM_SYSTEM_GP2X
