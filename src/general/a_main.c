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

#include "a_main.v.h"
#include <a2x.v.h>

#ifdef __GLIBC__
    #define A__BACKTRACE 1
    #include <execinfo.h>
#endif

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

static int g_argsNum;
static const char** g_args;

static const APack* g_packs[] = {
    &a_pack__console,
    &a_pack__embed,
    &a_pack__platform,
    &a_pack__block,
    &a_pack__timer,
    &a_pack__input,
    &a_pack__screen,
    &a_pack__color,
    &a_pack__fps,
    &a_pack__screenshot,
    &a_pack__sound,
    &a_pack__random,
    &a_pack__fix,
    &a_pack__state,
    &a_pack__ecs,
    &a_pack__fade,
    &a_pack__font,
};

static void a__atexit(void)
{
    a_out__info("Running atexit");

    for(unsigned pass = A_PACK__PASSES_NUM; pass--; ) {
        for(unsigned pack = A_ARRAY_LEN(g_packs); pack--; ) {
            if(g_packs[pack]->uninit[pass]) {
                a_out__info("[%s] Uninit pass %d", g_packs[pack]->name, pass);
                g_packs[pack]->uninit[pass]();
            }
        }
    }

    #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
        #if A_CONFIG_SYSTEM_GP2X_MENU
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        #endif
    #endif
}

int main(int Argc, char* Argv[])
{
    a_out__info("PID: %d", getpid());
    a_out__info("a2x: %s %s", A_CONFIG_BUILD_UID, A_CONFIG_BUILD_GIT_HASH);
    a_out__info("App: %s %s by %s",
                A_CONFIG_APP_TITLE,
                A_CONFIG_APP_VERSION_STRING,
                A_CONFIG_APP_AUTHOR);
    a_out__info("Build timestamp: %s", A_CONFIG_BUILD_TIMESTAMP);

    g_argsNum = Argc;
    g_args = (const char**)Argv;

    if(atexit(a__atexit)) {
        a_out__error("Cannot register atexit callback");
    }

    for(unsigned pass = 0; pass < A_PACK__PASSES_NUM; pass++) {
        for(unsigned pack = 0; pack < A_ARRAY_LEN(g_packs); pack++) {
            if(g_packs[pack]->init[pass]) {
                a_out__info("[%s] Init pass %d", g_packs[pack]->name, pass);
                g_packs[pack]->init[pass]();
            }
        }
    }

    a_out__info("a_main start");
    a_main();
    a_out__info("a_main end");

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
            a_time_spinSec(1);
        }
    #else
        if(a_console__isInitialized()) {
            for(int s = 10; s > 0; s--) {
                if(s == 10) {
                    a_out__info("Exiting in %ds", s);
                } else {
                    a_out__overwrite(A_OUT__TYPE_INFO,
                                     A_OUT__STREAM_STDOUT,
                                     "Exiting in %ds",
                                     s);
                }

                a_console__draw();
                a_screen__draw();
                a_time_waitSec(1);
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
