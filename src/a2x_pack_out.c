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
    #define A_OUT__HEADER(title, color) \
        "\033[" #color ";1m[ a2x " title " ]\033[0m "
#else
    #define A_OUT__HEADER(title, color) "[ a2x " title " ] "
#endif

#define A_OUT__WORKER(title, color, stream)       \
({                                                \
    va_list args;                                 \
    va_start(args, fmt);                          \
                                                  \
    fprintf(stream, A_OUT__HEADER(title, color)); \
    vfprintf(stream, fmt, args);                  \
    fprintf(stream, "\n");                        \
                                                  \
    va_end(args);                                 \
})

#define A_OUT__CONSOLE(type)           \
({                                     \
    char buffer[256];                  \
    va_list args;                      \
    va_start(args, fmt);               \
                                       \
    vsnprintf(buffer, 256, fmt, args); \
    a_console__write(type, buffer);    \
                                       \
    va_end(args);                      \
})

void a_out__init(void)
{
    //
}

void a_out__uninit(void)
{
    //
}

void a_out__message(char* fmt, ...)
{
    if(!a_settings_getBool("app.quiet")) {
        A_OUT__WORKER("Msg", 32, stdout);
        A_OUT__CONSOLE(A_CONSOLE_MESSAGE);
    }
}

void a_out__warning(char* fmt, ...)
{
    A_OUT__WORKER("Wrn", 33, stderr);
    A_OUT__CONSOLE(A_CONSOLE_WARNING);
}

void a_out__error(char* fmt, ...)
{
    A_OUT__WORKER("Err", 31, stderr);
    A_OUT__CONSOLE(A_CONSOLE_ERROR);
}

void a_out__fatal(char* fmt, ...)
{
    A_OUT__WORKER("Ftl", 35, stderr);
    exit(1);
}

void a_out__state(char* fmt, ...)
{
    if(!a_settings_getBool("app.quiet")) {
        A_OUT__WORKER("Stt", 34, stdout);
        A_OUT__CONSOLE(A_CONSOLE_STATE);
    }
}
