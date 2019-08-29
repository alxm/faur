/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_out.v.h"
#include <a2x.v.h>

typedef enum {
    A_OUT__FLAG_OVERWRITE = A_FLAGS_BIT(0),
} AOutFlags;

typedef enum {
    A_COLOR__INVALID = -1,
    A_COLOR__BLACK = 30,
    A_COLOR__RED = 31,
    A_COLOR__GREEN = 32,
    A_COLOR__YELLOW = 33,
    A_COLOR__BLUE = 34,
    A_COLOR__MAGENTA = 35,
    A_COLOR__CYAN = 36,
    A_COLOR__WHITE = 37
} AColorCode;

static const char* g_sources[A_OUT__SOURCE_NUM] = {
    [A_OUT__SOURCE_A2X] = "a2x",
    [A_OUT__SOURCE_APP] = "App",
};

static const struct {
    const char* name;
    AColorCode color;
} g_types[A_OUT__TYPE_NUM] = {
    [A_OUT__TYPE_INFO] = {"Inf", A_COLOR__GREEN},
    [A_OUT__TYPE_WARNING] = {"Wrn", A_COLOR__YELLOW},
    [A_OUT__TYPE_ERROR] = {"Err", A_COLOR__RED},
    [A_OUT__TYPE_STATE] = {"Stt", A_COLOR__BLUE},
    [A_OUT__TYPE_FATAL] = {"Ftl", A_COLOR__RED},
};

static void outWorkerPrint(AOutSource Source, AOutType Type, FILE* Stream, const char* Text)
{
    #if A_CONFIG_SYSTEM_LINUX && A_CONFIG_TRAIT_DESKTOP
        fprintf(Stream,
                "\033[1;%dm[%s][%s][%08x]\033[0m ",
                g_types[Type].color,
                g_sources[Source],
                g_types[Type].name,
                (uint32_t)a_fps_ticksGet());
    #else
        fprintf(Stream,
                "[%s][%s][%08x] ",
                g_sources[Source],
                g_types[Type].name,
                (uint32_t)a_fps_ticksGet());
    #endif

    fputs(Text, Stream);
    fputs("\n", Stream);

    a_console__write(Source, Type, Text);
}

static void outWorker(AOutSource Source, AOutType Type, FILE* Stream, const char* Format, va_list Args)
{
    #if !A_CONFIG_OUTPUT_ON
        return;
    #endif

    static char buffer[512];

    if(a_str_fmtv(buffer, sizeof(buffer), true, Format, Args)) {
        outWorkerPrint(Source, Type, Stream, buffer);
    }
}

void a_out__info(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_INFO,
              A_OUT__STREAM_STDOUT,
              Format,
              args);

    va_end(args);
}

void a_out__warning(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_WARNING,
              A_OUT__STREAM_STDERR,
              Format,
              args);

    va_end(args);
}

void a_out__error(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_ERROR,
              A_OUT__STREAM_STDERR,
              Format,
              args);

    va_end(args);
}

void a_out__errorv(const char* Format, va_list Args)
{
    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_ERROR,
              A_OUT__STREAM_STDERR,
              Format,
              Args);
}

void a_out__state(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_STATE,
              A_OUT__STREAM_STDOUT,
              Format,
              args);

    va_end(args);
}

void a_out_text(const char* Text)
{
    #if !A_CONFIG_OUTPUT_ON
        return;
    #endif

    outWorkerPrint(A_OUT__SOURCE_APP,
                   A_OUT__TYPE_INFO,
                   A_OUT__STREAM_STDOUT,
                   Text);
}

void a_out_info(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_INFO,
              A_OUT__STREAM_STDOUT,
              Format,
              args);

    va_end(args);
}

void a_out_warning(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_WARNING,
              A_OUT__STREAM_STDERR,
              Format,
              args);

    va_end(args);
}

void a_out_error(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_ERROR,
              A_OUT__STREAM_STDERR,
              Format,
              args);

    va_end(args);
}
