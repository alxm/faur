/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#include "a2x_pack_main.v.h"

#ifdef __GLIBC__
    #define A__BACKTRACE 1
    #include <execinfo.h>
#endif

#if A_BUILD_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_block.v.h"
#include "a2x_pack_conf.v.h"
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
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_time.v.h"
#include "a2x_pack_timer.v.h"

static int g_argsNum;
static const char** g_args;

static void a__atexit(void)
{
    a_out__message("Running atexit");

    a_out__message("A_EXIT start");
    a_exit();
    a_out__message("A_EXIT end");

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

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
        if(a_settings_boolGet(A_SETTING_SYSTEM_GP2X_MENU)) {
            a_settings__uninit();
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #endif

    a_settings__uninit();
}

int main(int Argc, char* Argv[])
{
    g_argsNum = Argc;
    g_args = (const char**)Argv;

    a_console__init();
    a_settings__init();

    a_out__message("A_SETUP start");
    a_setup();
    a_out__message("A_SETUP end");

    a_settings__init2();

    a_out__message("a2x: %s %s", A_BUILD__PLATFORM_NAME, A_BUILD__GIT_HASH);
    a_out__message("App: %s %s by %s",
                   a_settings_stringGet(A_SETTING_APP_TITLE),
                   a_settings_stringGet(A_SETTING_APP_VERSION),
                   a_settings_stringGet(A_SETTING_APP_AUTHOR));
    a_out__message("Build timestamp %s", A_BUILD__COMPILE_TIME);
    a_out__message("PID %d", getpid());

    a_embed__init();
    a_block__init();
    a_conf__init();
    a_platform__init();
    a_timer__init();
    a_input__init();
    a_screen__init();
    a_pixel__init();
    a_platform__init2();
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
    a_settings__init3();

    if(atexit(a__atexit)) {
        a_out__error("Cannot register atexit callback");
    }

    a_out__message("A_MAIN start");
    a_main();
    a_out__message("A_MAIN end");

    a_state__run();

    return 0;
}

int a_main_argsGetNum(void)
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

    a_settings_boolSet(A_SETTING_OUTPUT_CONSOLE, true);
    a_screen__draw();

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
                    a_out__overwrite(
                        A_OUT__TYPE_MESSAGE, stdout, "Exiting in %ds", s);
                }

                a_console__draw();
                a_screen__draw();
                a_time_secWait(1);
            }
        }
    #endif

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        emscripten_force_exit(1);
    #endif

    exit(1);
}

void A__FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    a_out__fatal(Format, args, false);

    va_end(args);

    handleFatal();
}

void A_FATAL(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    a_out__fatal(Format, args, true);

    va_end(args);

    handleFatal();
}
