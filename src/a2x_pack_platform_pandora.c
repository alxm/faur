/*
    Copyright 2016, 2017 Alex Margarit

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

#include "a2x_system_includes.h"

#if A_PLATFORM_SYSTEM_PANDORA
#include "a2x_pack_file.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform_pandora.v.h"
#include "a2x_pack_str.v.h"

static struct {
    char* nub0Mode;
    char* nub1Mode;
} g_pandora;

static void pandora_setNubModes(const char* Nub0, const char* Nub1)
{
    char cmd[128];
    int r = snprintf(cmd,
                     sizeof(cmd),
                     "/usr/pandora/scripts/op_nubchange.sh %s %s",
                     Nub0,
                     Nub1);

    if(r < 0 || r >= (int)sizeof(cmd)) {
        a_out__error("pandora_setNubModes snprintf failed");
        return;
    }

    if(system(cmd) < 0) {
        a_out__error("op_nubchange.sh failed");
        return;
    }

    a_out__message("Set nub modes to %s, %s", Nub0, Nub1);
}

static void pandora_setScreenFilter(const char* Value)
{
    char cmd[128];
    int r = snprintf(cmd,
                     sizeof(cmd),
                     "sudo -n /usr/pandora/scripts/op_videofir.sh %s",
                     Value);

    if(r < 0 || r >= (int)sizeof(cmd)) {
        a_out__error("pandora_setScreenFilter snprintf failed");
        return;
    }

    if(system(cmd) < 0) {
        a_out__error("op_videofir.sh failed");
        return;
    }

    a_out__message("Set screen filter to %s", Value);
}

void a_platform_pandora__init(void)
{
    g_pandora.nub0Mode = NULL;
    g_pandora.nub1Mode = NULL;

    AFile* nub0 = a_file_open("/proc/pandora/nub0/mode", "r");
    AFile* nub1 = a_file_open("/proc/pandora/nub1/mode", "r");

    if(nub0 && nub1 && a_file_readLine(nub0) && a_file_readLine(nub1)) {
        g_pandora.nub0Mode = a_str_dup(a_file_getLine(nub0));
        a_out__message("Current nub0Mode is '%s'", g_pandora.nub0Mode);

        g_pandora.nub1Mode = a_str_dup(a_file_getLine(nub1));
        a_out__message("Current nub1Mode is '%s'", g_pandora.nub1Mode);
    }

    if(nub0) {
        a_file_close(nub0);
    }

    if(nub1) {
        a_file_close(nub1);
    }

    // Gamepad mode
    pandora_setNubModes("absolute", "absolute");

    // Crisp pixels, no blur filter
    pandora_setScreenFilter("none");
}

void a_platform_pandora__uninit(void)
{
    if(g_pandora.nub0Mode && g_pandora.nub1Mode) {
        pandora_setNubModes(g_pandora.nub0Mode, g_pandora.nub1Mode);
    }

    free(g_pandora.nub0Mode);
    free(g_pandora.nub1Mode);

    pandora_setScreenFilter("default");
}
#endif // A_PLATFORM_SYSTEM_PANDORA
