/*
    Copyright 2016 Alex Margarit

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

#include "a2x_pack_out.v.h"

#if A_PLATFORM_LINUXPC
    #define A_OUT__HEADER(title, color) "\033[" #color ";1m[ a2x " title " ]\033[0m "
#else
    #define A_OUT__HEADER(title, color) "[ a2x " title " ] "
#endif

void a_out__init(void)
{
    //
}

void a_out__uninit(void)
{
    //
}

void a_out(char* fmt, ...)
{
    if(!a2x_bool("app.quiet")) {
        va_list args;
        va_start(args, fmt);

        printf(A_OUT__HEADER("Msg", 32));
        vprintf(fmt, args);
        printf("\n");

        va_end(args);
    }
}

void a_warning(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, A_OUT__HEADER("Wrn", 33));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

void a_error(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, A_OUT__HEADER("Wrn", 31));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

void a_fatal(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, A_OUT__HEADER("Wrn", 35));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}
