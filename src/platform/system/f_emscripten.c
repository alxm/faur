/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#include "f_emscripten.v.h"
#include <faur.v.h>

#if F_CONFIG_SYSTEM_EMSCRIPTEN
#include <emscripten.h>

void f_platform_emscripten__init(void)
{
    EM_ASM(
        {
            // Strip trailing slash
            var prefix = UTF8ToString($0).slice(0, -1);

            Module.faur_fsIsReady = 0;

            FS.mkdir(prefix);
            FS.mount(IDBFS, {}, prefix);
            FS.syncfs(
                true,
                function(Error)
                {
                    if(Error) {
                        Module.printErr("Could not init IDBFS");
                        Module.faur_fsIsReady = 1;
                    } else {
                        Module.faur_fsIsReady = 2;
                    }
                }
            );
        },
        F_CONFIG_FILES_PREFIX
    );
}

void f_platform_api__customExit(int Status)
{
    emscripten_force_exit(Status);
}

static void f__loop(void)
{
    if(EM_ASM_INT({ return Module.faur_fsIsReady === 0; })) {
        return;
    }

    if(!f_state__runStep()) {
        emscripten_cancel_main_loop();
    }
}

int main(int Argc, char* Argv[])
{
    F_UNUSED(Argc);
    F_UNUSED(Argv);

    f__main();

    emscripten_set_main_loop(
        f__loop,
        f_platform_api__screenVsyncGet() ? 0 : F_CONFIG_FPS_RATE_DRAW,
        true);

    return 0;
}

FVecInt f_platform_emscripten__windowSizeGet(void)
{
    return (FVecInt){
        EM_ASM_INT({
            return window.innerWidth;
        }),
        EM_ASM_INT({
            return window.innerHeight;
        })
    };
}

void f_platform_api__fileSync(void)
{
    EM_ASM({
        if(Module.faur_fsIsReady === 2) {
            FS.syncfs(false, function(Error) {});
        }
    });
}
#endif // F_CONFIG_SYSTEM_EMSCRIPTEN
