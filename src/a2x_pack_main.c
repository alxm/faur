/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "a2x_pack_main.v.h"

#ifdef __GLIBC__
    #define A__BACKTRACE 1
    #include <execinfo.h>
#endif

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_block.v.h"
#include "a2x_pack_console.v.h"
#include "a2x_pack_ecs.v.h"
#include "a2x_pack_embed.v.h"
#include "a2x_pack_fade.v.h"
#include "a2x_pack_fix.v.h"
#include "a2x_pack_font.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_random.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_time.v.h"
#include "a2x_pack_timer.v.h"

static int g_argsNum;
static const char** g_args;

static void a__atexit(void)
{
    a_out__message("Running atexit");

    a_console__uninit();
    a_font__uninit();
    a_fade__uninit();
    a_ecs__uninit();
    a_state__uninit();
    a_sound__uninit();
    a_screenshot__uninit();
    a_fps__uninit();
    a_pixel__uninit();
    a_screen__uninit();
    a_input__uninit();
    a_timer__uninit();
    a_platform__uninit();
    a_block__uninit();
    a_embed__uninit();

    #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
        #if A_CONFIG_SYSTEM_GP2X_MENU
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        #endif
    #endif
}

int main(int Argc, char* Argv[])
{
    a_out__message("PID: %d", getpid());
    a_out__message("a2x: %s %s", A_CONFIG_BUILD_UID, A_CONFIG_BUILD_GIT_HASH);
    a_out__message("App: %s %s by %s",
                   A_CONFIG_APP_TITLE,
                   A_CONFIG_APP_VERSION_STRING,
                   A_CONFIG_APP_AUTHOR);
    a_out__message("Build timestamp: %s", A_CONFIG_BUILD_TIMESTAMP);

    g_argsNum = Argc;
    g_args = (const char**)Argv;

    a_console__init();
    a_embed__init();
    a_block__init();
    a_platform__init();
    a_timer__init();
    a_input__init();
    a_screen__init();
    a_pixel__init();
    a_sprite__init();
    a_fps__init();
    a_screenshot__init();
    a_sound__init();
    a_random__init();
    a_fix__init();
    a_state__init();
    a_ecs__init();
    a_fade__init();
    a_font__init();
    a_console__init2();

    if(atexit(a__atexit)) {
        a_out__error("Cannot register atexit callback");
    }

    a_out__message("A_MAIN start");
    a_main();
    a_out__message("A_MAIN end");

    a_state__run();

    return 0;
}

int a_main_argsNumGet(void)
{
    return g_argsNum;
}

const char* a_main_argsGet(int ArgNum)
{
    if(ArgNum >= g_argsNum) {
        A__FATAL("a_main_argsGet(%d): Only %d args total", ArgNum, g_argsNum);
    }

    return g_args[ArgNum];
}

__attribute__((noreturn)) static void handleFatal(void)
{
    #if A__BACKTRACE
        void* addresses[16];
        int numAddresses = backtrace(addresses, A_ARRAY_LEN(addresses));
        char** functionNames = backtrace_symbols(addresses, numAddresses);

        for(int i = 0; i < numAddresses; i++) {
            a_out__error(functionNames[i]);
        }

        free(functionNames);
    #endif

    a_console_showSet(true);
    a_console__draw();
    a_screen__draw();

    #if A_CONFIG_BUILD_DEBUG_WAIT
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
                    a_out__overwrite(A_OUT__TYPE_MESSAGE,
                                     A_OUT__STREAM_STDOUT,
                                     "Exiting in %ds",
                                     s);
                }

                a_console__draw();
                a_screen__draw();
                a_time_secWait(1);
            }
        }
    #endif

    #if A_CONFIG_SYSTEM_EMSCRIPTEN
        emscripten_force_exit(1);
    #endif

    exit(1);
}

void A__FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    a_out__errorv(Format, args);

    va_end(args);

    handleFatal();
}

void A_FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    a_out__errorv(Format, args);

    va_end(args);

    handleFatal();
}
