/*
    Copyright 2015 Alex Margarit <alex@alxm.org>
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

#include "f_errors.v.h"
#include <faur.v.h>

F__ATTRIBUTE_NORETURN static void handleFatal(FOutSource Source)
{
    f_out__backtrace(Source);

    #if F_CONFIG_DEBUG_FATAL_SPIN
        while(true);
    #else
        #if F_CONFIG_OUT_CONSOLE_ENABLED
            if(f_console__isInitialized()) {
                f_console_showSet(true);
                f_console__draw();
                f_screen__draw();
            }
        #endif

        #if F_CONFIG_DEBUG_FATAL_WAIT
            while(true) {
                printf("Waiting to attach debugger: PID %d\n", getpid());
                f_time_msSpin(1000);
            }
        #elif !F_CONFIG_TRAIT_DESKTOP
            #if F_CONFIG_OUT_CONSOLE_ENABLED
                if(f_console__isInitialized()) {
                    f_out__info("Exiting in 10s");
                    f_console__draw();
                    f_screen__draw();
                    f_time_msWait(10 * 1000);
                }
            #endif
        #endif

        f_platform_api__customExit(EXIT_FAILURE);
        exit(EXIT_FAILURE);
    #endif
}

void F__FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    f_out__errorv(Format, args);

    va_end(args);

    handleFatal(F_OUT__SOURCE_FAUR);
}

void F_FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    f_out_errorv(Format, args);

    va_end(args);

    handleFatal(F_OUT__SOURCE_APP);
}
