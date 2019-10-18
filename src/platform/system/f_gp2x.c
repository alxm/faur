/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org> and:

    - GP2X clock speed from the GP2X Wiki
    - GP2X mmuhack by Squidge and NK
    - GP2X ram timings by JyCet

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

#include "f_gp2x.v.h"
#include <faur.v.h>

#if A_CONFIG_SYSTEM_GP2X
#include <fcntl.h>
#include <sys/mman.h>

#if A_CONFIG_SYSTEM_GP2X_MHZ > 0
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
#endif

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

void f_platform_gp2x__init(void)
{
    if(f_path_exists("./mmuhack.o", A_PATH_FILE)) {
        system("/sbin/rmmod mmuhack");
        system("/sbin/insmod mmuhack.o");
    }

    #if A_CONFIG_SYSTEM_GP2X_MHZ > 0
        setCpuSpeed(A_CONFIG_SYSTEM_GP2X_MHZ);
    #endif

    setRamTimings(6, 4, 1, 1, 1, 2, 2);
}

void f_platform_gp2x__uninit(void)
{
    int mmufd = open("/dev/mmuhack", O_RDWR);

    if(mmufd >= 0) {
        close(mmufd);
        system("/sbin/rmmod mmuhack");
    }

    setRamTimings(8, 16, 3, 8, 8, 8, 8);
}
#endif // A_CONFIG_SYSTEM_GP2X
