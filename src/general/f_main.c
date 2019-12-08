/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_main.v.h"
#include <faur.v.h>

#ifdef __GLIBC__
    #define F__BACKTRACE 1
    #include <execinfo.h>
#endif

#if F_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

static int g_argsNum;
static const char** g_args;

static void f__atexit(void)
{
    f_out__info("Running atexit");

    f_init__uninit();

    #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        #if F_CONFIG_SYSTEM_GP2X_MENU
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        #endif
    #endif
}

#if F_CONFIG_BUILD_MAIN
int main(int Argc, char* Argv[])
{
    g_argsNum = Argc;
    g_args = (const char**)Argv;

    f__main();

    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        emscripten_set_main_loop(
            f_platform_emscripten__loop,
            f_platform_api__screenVsyncGet() ? 0 : F_CONFIG_FPS_RATE_DRAW,
            true);
    #else
        while(f_state__runStep()) {
            continue;
        }
    #endif

    return 0;
}
#endif

void f__main(void)
{
    f_init();

    f_out__info("PID: %d", getpid());
    f_out__info("Faur: %s %s", F_CONFIG_BUILD_UID, F_CONFIG_BUILD_FAUR_GIT);
    f_out__info("App: %s %s by %s",
                f_init__app_name,
                f_init__app_version,
                f_init__app_author);
    f_out__info("Build timestamp: %s", F_CONFIG_BUILD_FAUR_TIME);

    if(atexit(f__atexit)) {
        f_out__error("Cannot register atexit callback");
    }

    f_init__init();

    f_main();
}

int f_main_argsNumGet(void)
{
    return g_argsNum;
}

const char* f_main_argsGet(int ArgNum)
{
    if(ArgNum >= g_argsNum) {
        F__FATAL("f_main_argsGet(%d): Only %d args total", ArgNum, g_argsNum);
    }

    return g_args[ArgNum];
}

__attribute__((noreturn)) static void handleFatal(void)
{
    #if F_CONFIG_BUILD_DEBUG_FATAL_SPIN
        while(true);
    #else
        #if F__BACKTRACE
            void* addresses[16];
            int numAddresses = backtrace(addresses, F_ARRAY_LEN(addresses));
            char** functionNames = backtrace_symbols(addresses, numAddresses);

            for(int i = 0; i < numAddresses; i++) {
                f_out__error(functionNames[i]);
            }

            free(functionNames);
        #endif

        f_console_showSet(true);
        f_console__draw();
        f_screen__draw();

        #if F_CONFIG_BUILD_DEBUG_WAIT
            while(true) {
                printf("Waiting to attach debugger: PID %d\n", getpid());
                f_time_spinSec(1);
            }
        #elif !F_CONFIG_TRAIT_DESKTOP
            if(f_console__isInitialized()) {
                f_out__info("Exiting in 10s");
                f_console__draw();
                f_screen__draw();
                f_time_waitSec(10);
            }
        #endif

        #if F_CONFIG_SYSTEM_EMSCRIPTEN
            emscripten_force_exit(1);
        #endif

        exit(EXIT_FAILURE);
    #endif
}

void F__FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    f_out__errorv(Format, args);

    va_end(args);

    handleFatal();
}

void F_FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    f_out__errorv(Format, args);

    va_end(args);

    handleFatal();
}
