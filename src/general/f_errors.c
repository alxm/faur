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

F__ATTRIBUTE_NORETURN static void handleFatal(const char* Format, va_list Args)
{
    static bool g_handleFatalInProgress;

    if(!g_handleFatalInProgress) {
        g_handleFatalInProgress = true;

        f_color_reset();
        f_draw_fill();

        f_font_reset();
        f_font_lineWrapSet(f_screen_sizeGetWidth());
        f_font_printv(Format, Args);

        f_screen__draw();

        #if F_CONFIG_DEBUG_FATAL == F_DEBUG_FATAL_SEGFAULT
            *(volatile int*)(NULL) = 0;
        #elif F_CONFIG_DEBUG_FATAL == F_DEBUG_FATAL_SPIN
            while(true);
        #elif F_CONFIG_DEBUG_FATAL == F_DEBUG_FATAL_WAIT
            while(true) {
                printf("Waiting to attach debugger: PID %d\n", getpid());
                f_time_msSpin(1000);
            }
        #else
            f_time_msWait(10 * 1000);
        #endif
    }

    f_platform_api__customExit(EXIT_FAILURE);
    exit(EXIT_FAILURE);
}

void F__FATAL(const char* Format, ...)
{
    va_list args;

    va_start(args, Format);
    f_out__errorv(Format, args);
    va_end(args);

    va_start(args, Format);
    handleFatal(Format, args);
    va_end(args);
}

void F_FATAL(const char* Format, ...)
{
    F__CHECK(Format != NULL);

    va_list args;

    va_start(args, Format);
    f_out_errorv(Format, args);
    va_end(args);

    va_start(args, Format);
    handleFatal(Format, args);
    va_end(args);
}
