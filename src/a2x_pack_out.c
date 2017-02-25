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
    #define A_OUT__HEADER(Title, Color) \
        "\033[" #Color ";1m[ a2x " Title " ]\033[0m "
#else
    #define A_OUT__HEADER(Title, Color) \
        "[ a2x " Title " ] "
#endif

#define A_OUT__WORKER(Title, Color, Stream)       \
{                                                 \
    va_list args;                                 \
    va_start(args, Format);                       \
                                                  \
    fprintf(Stream, A_OUT__HEADER(Title, Color)); \
    vfprintf(Stream, Format, args);               \
    fprintf(Stream, "\n");                        \
                                                  \
    va_end(args);                                 \
}

#define A_OUT__CONSOLE(Type)              \
{                                         \
    char buffer[256];                     \
    va_list args;                         \
    va_start(args, Format);               \
                                          \
    vsnprintf(buffer, 256, Format, args); \
    a_console__write(Type, buffer);       \
                                          \
    va_end(args);                         \
}

void a_out__message(const char* Format, ...)
{
    if(a_settings_getBool("app.output.on")) {
        A_OUT__WORKER("Msg", 32, stdout);
        A_OUT__CONSOLE(A_CONSOLE_MESSAGE);
    }
}

void a_out__warning(const char* Format, ...)
{
    A_OUT__WORKER("Wrn", 33, stderr);
    A_OUT__CONSOLE(A_CONSOLE_WARNING);
}

void a_out__error(const char* Format, ...)
{
    A_OUT__WORKER("Err", 31, stderr);
    A_OUT__CONSOLE(A_CONSOLE_ERROR);
}

void a_out__fatal(const char* Format, ...)
{
    A_OUT__WORKER("Ftl", 35, stderr);
    exit(1);
}

void a_out__state(const char* Format, ...)
{
    if(a_settings_getBool("app.output.on")) {
        A_OUT__WORKER("Stt", 34, stdout);
        A_OUT__CONSOLE(A_CONSOLE_STATE);
    }
}

void a_out__stateVerbose(const char* Format, ...)
{
    if(a_settings_getBool("app.output.on")
        && a_settings_getBool("app.output.verbose")) {

        A_OUT__WORKER("Stt", 34, stdout);
        A_OUT__CONSOLE(A_CONSOLE_STATE);
    }
}
