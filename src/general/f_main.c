/*
    Copyright 2010, 2016-2021 Alex Margarit <alex@alxm.org>
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

#include <unistd.h>

#ifdef __GLIBC__
    #define F__BACKTRACE 1
    #include <execinfo.h>
#endif

static int g_argsNum;
static const char** g_args;

#if !F_CONFIG_TRAIT_CUSTOM_MAIN
int main(int Argc, char* Argv[])
{
    g_argsNum = Argc;
    g_args = (const char**)Argv;

    f__main();

    while(f_state__runStep()) {
        continue;
    }

    f_init__uninit();

    return 0;
}
#endif

void f__main(void)
{
    f_out__info("PID: %d", getpid());
    f_out__info("Faur: %s %s", F_CONFIG_BUILD_UID, F_CONFIG_BUILD_FAUR_GIT);
    f_out__info("App: %s %s by %s",
                F_CONFIG_APP_NAME,
                F__APP_VERSION_STRING,
                F_CONFIG_APP_AUTHOR);
    f_out__info("Build timestamp: %s", F_CONFIG_BUILD_FAUR_TIME);

    f_init__init();
    f_state_push(f_main);
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

F__ATTRIBUTE_NORETURN static void handleFatal(void)
{
    #if F_CONFIG_DEBUG_FATAL_SPIN
        while(true);
    #else
        #if F__BACKTRACE
            void* addresses[16];
            int numAddresses = backtrace(addresses, F_ARRAY_LEN(addresses));
            char** functionNames = backtrace_symbols(addresses, numAddresses);

            for(int i = 0; i < numAddresses; i++) {
                f_out__error("%s", functionNames[i]);
            }

            free(functionNames);
        #endif

        #if F_CONFIG_CONSOLE_ENABLED
            if(f_console__isInitialized()) {
                f_console_showSet(true);
                f_console__draw();
                f_screen__draw();
            }
        #endif

        #if F_CONFIG_DEBUG_WAIT
            while(true) {
                printf("Waiting to attach debugger: PID %d\n", getpid());
                f_time_msSpin(1000);
            }
        #elif !F_CONFIG_TRAIT_DESKTOP
            #if F_CONFIG_CONSOLE_ENABLED
                if(f_console__isInitialized()) {
                    f_out__info("Exiting in 10s");
                    f_console__draw();
                    f_screen__draw();
                    f_time_msWait(10 * 1000);
                }
            #endif
        #endif

        #if F_CONFIG_TRAIT_CUSTOM_EXIT
            f_platform_api__customExit(EXIT_FAILURE);
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
