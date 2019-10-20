/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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
            FS.syncfs(true, function(Error) { Module.faur_fsIsReady = 1; });
        },
        F_CONFIG_PATH_STORAGE_PREFIX
    );
}
#endif // F_CONFIG_SYSTEM_EMSCRIPTEN
