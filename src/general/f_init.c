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

#include "f_init.v.h"
#include <faur.v.h>

#include <unistd.h>

bool f_init__done;

static const FPack* g_packs[] = {
    &f_pack__pool,
#if F_CONFIG_TRAIT_CONSOLE
    &f_pack__console_0,
#endif
    &f_pack__embed,
    &f_pack__platform,
    &f_pack__screen,
    &f_pack__align,
    &f_pack__color,
    &f_pack__font,
    &f_pack__fps,
#if F_CONFIG_TRAIT_SCREENSHOTS
    &f_pack__screenshot,
#endif
    &f_pack__sound,
    &f_pack__random,
#if F_CONFIG_BUILD_GEN_LUTS
    &f_pack__fix,
    &f_pack__hash,
#endif
    &f_pack__state,
    &f_pack__ecs,
    &f_pack__fade,
#if F_CONFIG_TRAIT_CONSOLE
    &f_pack__console_1,
#endif
};

void f_init__init(void)
{
    f_out__info("Faur: %s %s", F_CONFIG_BUILD_UID, F_CONFIG_BUILD_FAUR_GIT);
    f_out__info("App: %s %s by %s",
                F_CONFIG_APP_NAME,
                F__APP_VERSION_STRING,
                F_CONFIG_APP_AUTHOR);
    f_out__info("Build timestamp: %s", F_CONFIG_BUILD_FAUR_TIME);
    f_out__info("PID: %d", getpid());
    f_out__info("Start init");

    unsigned num = F_ARRAY_LEN(g_packs);

    for(unsigned p = 0; p < num; p++) {
        const FPack* pack = g_packs[p];

        if(pack->init != NULL) {
            f_out__info("[Init %02u/%02u] %s", p + 1, num, pack->name);
            pack->init();
        }
    }

    f_init__done = true;

    f_out__info("Init complete");
}

void f_init__uninit(void)
{
    f_out__info("Start uninit");

    f_init__done = false;

    unsigned num = F_ARRAY_LEN(g_packs);

    for(unsigned p = num; p--; ) {
        const FPack* pack = g_packs[p];

        if(pack->uninit != NULL) {
            f_out__info("[Uninit %02u/%02u] %s", p + 1, num, pack->name);
            pack->uninit();
        }
    }

    f_out__info("Uninit complete");
}
