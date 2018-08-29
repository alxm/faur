/*
    Copyright 2016-2018 Alex Margarit

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

#if A_BUILD_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_console.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_time.v.h"

typedef enum {
    A_COLOR_INVALID = -1,
    A_COLOR_BLACK = 30,
    A_COLOR_RED = 31,
    A_COLOR_GREEN = 32,
    A_COLOR_YELLOW = 33,
    A_COLOR_BLUE = 34,
    A_COLOR_MAGENTA = 35,
    A_COLOR_CYAN = 36,
    A_COLOR_WHITE = 37
} AColorCode;

static const char* g_sources[A_OUT__SOURCE_NUM] = {
    [A_OUT__SOURCE_A2X] = "a2x",
    [A_OUT__SOURCE_APP] = "App",
};

static const struct {
    const char* name;
    AColorCode color;
} g_types[A_OUT__TYPE_NUM] = {
    [A_OUT__TYPE_MESSAGE] = {"Msg", A_COLOR_GREEN},
    [A_OUT__TYPE_WARNING] = {"Wrn", A_COLOR_YELLOW},
    [A_OUT__TYPE_ERROR] = {"Err", A_COLOR_RED},
    [A_OUT__TYPE_STATE] = {"Stt", A_COLOR_BLUE},
    [A_OUT__TYPE_FATAL] = {"Ftl", A_COLOR_MAGENTA},
};

static void outPrintHeader(AOutSource Source, AOutType Type, FILE* Stream)
{
    #if A_BUILD_SYSTEM_LINUX && A_BUILD_SYSTEM_DESKTOP
        fprintf(Stream,
                "\033[1;%dm[%s][%s][%u]\033[0m ",
                g_types[Type].color,
                g_sources[Source],
                g_types[Type].name,
                a_fps_ticksGet());
    #else
        fprintf(Stream,
                "[%s][%s][%u] ",
                g_sources[Source],
                g_types[Type].name,
                a_fps_ticksGet());
    #endif
}

static void outWorker(AOutSource Source, AOutType Type, bool Verbose, bool Overwrite, FILE* Stream, const char* Format, va_list Args)
{
    if(!a_settings_getBool("app.output.on")) {
        return;
    }

    if(Verbose && !a_settings_getBool("app.output.verbose")) {
        return;
    }

    char buffer[256];

    if(vsnprintf(buffer, sizeof(buffer), Format, Args) <= 0) {
        return;
    }

    outPrintHeader(Source, Type, Stream);
    fputs(buffer, Stream);
    fputs("\n", Stream);

    a_console__write(Source, Type, buffer, Overwrite);
}

void a_out__message(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_MESSAGE,
              false,
              false,
              stdout,
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
              false,
              false,
              stderr,
              Format,
              args);

    va_end(args);
}

void a_out__warningv(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_WARNING,
              true,
              false,
              stderr,
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
              false,
              false,
              stderr,
              Format,
              args);

    va_end(args);
}

void a_out__errorv(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_ERROR,
              true,
              false,
              stderr,
              Format,
              args);

    va_end(args);
}

void a_out__fatal(const char* Format, ...)
{
    if(!a_settings_getBool("app.output.on")) {
        a_settings_set("app.output.on", "y");
    }

    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_FATAL,
              false,
              false,
              stderr,
              Format,
              args);

    va_end(args);

    a_console__showSet(true);
    a_screen__show();

    #if A_BUILD_DEBUG
        while(true) {
            printf("Waiting to attach debugger: PID %d\n", getpid());
            a_time_secSpin(1);
        }
    #else
        if(a_console__isInitialized()) {
            for(int s = 10; s > 0; s--) {
                if(s == 10) {
                    a_out__message("Exiting in %ds", s);
                } else {
                    a_out__overwrite(A_OUT__TYPE_MESSAGE, "Exiting in %ds", s);
                }

                a_screen__show();
                a_time_secWait(1);
            }
        }
    #endif

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        emscripten_force_exit(1);
    #endif

    exit(1);
}

void a_out__state(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_STATE,
              false,
              false,
              stderr,
              Format,
              args);

    va_end(args);
}

void a_out__statev(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              A_OUT__TYPE_STATE,
              true,
              false,
              stderr,
              Format,
              args);

    va_end(args);
}

void a_out__overwrite(AOutType Type, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_A2X,
              Type,
              false,
              true,
              stdout,
              Format,
              args);

    va_end(args);
}

void a_out_print(const char* Text)
{
    if(a_settings_getBool("app.output.on")) {
        outPrintHeader(A_OUT__SOURCE_APP, A_OUT__TYPE_MESSAGE, stdout);
        puts(Text);
    }
}

void a_out_printf(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_MESSAGE,
              false,
              false,
              stdout,
              Format,
              args);

    va_end(args);
}

void a_out_printv(const char* Format, va_list Args)
{
    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_MESSAGE,
              false,
              false,
              stdout,
              Format,
              Args);
}

void a_out_warning(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    outWorker(A_OUT__SOURCE_APP,
              A_OUT__TYPE_WARNING,
              false,
              false,
              stdout,
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
              false,
              false,
              stdout,
              Format,
              args);

    va_end(args);
}
