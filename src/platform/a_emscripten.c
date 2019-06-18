/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#include "platform/a_emscripten.v.h"

#if A_CONFIG_SYSTEM_EMSCRIPTEN
#include <emscripten.h>

void a_platform_emscripten__init(void)
{
    EM_ASM(
        {
            // Strip trailing slash
            var prefix = UTF8ToString($0).slice(0, -1);

            Module.a2x_fsIsReady = 0;

            FS.mkdir(prefix);
            FS.mount(IDBFS, {}, prefix);
            FS.syncfs(true, function(Error) { Module.a2x_fsIsReady = 1; });
        },
        A_CONFIG_PATH_STORAGE_PREFIX
    );
}
#endif // A_CONFIG_SYSTEM_EMSCRIPTEN
