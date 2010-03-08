/*
    Copyright 2010 Alex Margarit

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
    Many thanks:

    - to a wiki poster for the GP2X clock speed setter
    - to Squidge and NK for the GP2X mmuhack
    - to Notaz for the Wiz mmuhack
    - to JyCet for the GP2X ram timings code
*/

#include "a2x_pack_hw.p.h"
#include "a2x_pack_hw.v.h"

#if A_PLATFORM_GP2X
    static void a__hw_cpu(const int Mhz);
    static void a__hw_ramTimings(int tRC, int tRAS, int tWR, int tMRD, int tRFC, int tRP, int tRCD);
#endif

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static int mmuHackOn = 0;
#endif

void a_hw__set(void)
{
    #if A_PLATFORM_GP2X
        if(a_file_exists("./mmuhack.o")) {
            system("/sbin/rmmod mmuhack");
            system("/sbin/insmod mmuhack.o");

            int mmufd = open("/dev/mmuhack", O_RDWR);

            if(mmufd >= 0) {
                close(mmufd);
                mmuHackOn = 1;
            }
        }

        if(a2x_int("mhz") > 0) {
            a__hw_cpu(a2x_int("mhz"));
        }

        a__hw_ramTimings(6, 4, 1, 1, 1, 2, 2);
    #elif A_PLATFORM_WIZ
        if(a_file_exists("./mmuhack.ko")) {
            system("/sbin/rmmod mmuhack");
            system("/sbin/insmod mmuhack.ko");

            int mmufd = open("/dev/mmuhack", O_RDWR);

            if(mmufd >= 0) {
                close(mmufd);
                mmuHackOn = 1;
            }
        }
    #endif
}

void a_hw__free(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(mmuHackOn) {
            system("/sbin/rmmod mmuhack");
        }
    #endif

    #if A_PLATFORM_GP2X
        a__hw_ramTimings(8, 16, 3, 8, 8, 8, 8);
    #endif
}

#if A_PLATFORM_GP2X
    static void a__hw_cpu(const int Mhz)
    {
        int mhz = Mhz * 1000000;
        int freq = 7372800;

        int memfd = open("/dev/mem", O_RDWR);
        volatile uint32_t* memregs32 = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0xc0000000);
        volatile uint16_t* memregs16 = (uint16_t*)memregs32;

        unsigned int scale = 0;
        unsigned int pdiv = 3;
        unsigned int mdiv = (mhz * pdiv) / freq;

        scale &= 3;
        pdiv = ((pdiv - 2) << 2) & 0xfc;
        mdiv = ((mdiv - 8) << 8) & 0xff00;

        unsigned int v = scale | pdiv | mdiv;

        unsigned int l = memregs32[0x808 >> 2]; // Get interupt flags
        memregs32[0x808 >> 2] = 0xFF8FFFE7; // Turn off interrupts
        memregs16[0x910 >> 1] = v; // Set frequentie
        while(memregs16[0x0902 >> 1] & 1) continue; // Wait for the frequentie to be ajused
        memregs32[0x808 >> 2] = l; // Turn on interrupts

        close(memfd);
    }

    static void a__hw_ramTimings(int tRC, int tRAS, int tWR, int tMRD, int tRFC, int tRP, int tRCD)
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
