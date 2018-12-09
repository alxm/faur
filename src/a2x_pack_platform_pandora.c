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

#include "a2x_pack_platform_pandora.v.h"

#if A_BUILD_SYSTEM_PANDORA
#include "a2x_pack_file.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

static char* g_nubModes[2];

static void pandora_setNubModes(const char* Nub0, const char* Nub1)
{
    const char* cmd = a_str_fmt(
                        "/usr/pandora/scripts/op_nubchange.sh %s %s",
                        Nub0,
                        Nub1);

    if(cmd == NULL) {
        return;
    }

    a_out__message("Set nub modes to '%s', '%s'", Nub0, Nub1);

    if(system(cmd) < 0) {
        a_out__error("%s: failed", cmd);
        return;
    }
}

static void pandora_setScreenFilter(const char* Value)
{
    const char* cmd = a_str_fmt(
                        "sudo -n /usr/pandora/scripts/op_videofir.sh %s",
                        Value);

    if(cmd == NULL) {
        return;
    }

    a_out__message("Set screen filter to '%s'", Value);

    if(system(cmd) < 0) {
        a_out__error("%s: failed", cmd);
        return;
    }
}

void a_platform_pandora__init(void)
{
    for(int i = 0; i < 2; i++) {
        const char* path = a_str_fmt("/proc/pandora/nub%d/mode", i);
        AFile* nub = a_file_new(path, A_FILE_READ);

        if(nub && a_file_lineRead(nub)) {
            g_nubModes[i] = a_str_dup(a_file_lineBufferGet(nub));
            a_out__message("Current nub%d mode is '%s'", i, g_nubModes[i]);
        }

        a_file_free(nub);
    }

    // Gamepad mode
    pandora_setNubModes("absolute", "absolute");

    // Crisp pixels, no blur filter
    pandora_setScreenFilter("none");
}

void a_platform_pandora__uninit(void)
{
    pandora_setScreenFilter("default");

    if(g_nubModes[0] && g_nubModes[1]) {
        pandora_setNubModes(g_nubModes[0], g_nubModes[1]);
    }

    for(int i = 0; i < 2; i++) {
        free(g_nubModes[i]);
    }
}
#endif // A_BUILD_SYSTEM_PANDORA
