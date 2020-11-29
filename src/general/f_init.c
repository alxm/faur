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

#include "f_init.v.h"
#include <faur.v.h>

static const FPack* g_packs[] = {
    &f_pack__pool,
    &f_pack__console_0,
    &f_pack__embed,
    &f_pack__platform,
    &f_pack__timer,
    &f_pack__screen,
    &f_pack__align,
    &f_pack__color,
    &f_pack__fps,
    &f_pack__screenshot,
    &f_pack__sound,
    &f_pack__random,
    &f_pack__fix,
    &f_pack__hash,
    &f_pack__state,
    &f_pack__sym,
    &f_pack__ecs,
    &f_pack__fade,
    &f_pack__font,
    &f_pack__console_1,
};

void f_init__init(void)
{
    unsigned num = F_ARRAY_LEN(g_packs);

    for(unsigned p = 0; p < num; p++) {
        const FPack* pack = g_packs[p];

        if(pack->init != NULL) {
            f_out__info("[Init %02u/%02u] %s", p + 1, num, pack->name);
            pack->init();
        }
    }
}

void f_init__uninit(void)
{
    unsigned num = F_ARRAY_LEN(g_packs);

    for(unsigned p = num; p--; ) {
        const FPack* pack = g_packs[p];

        if(pack->uninit != NULL) {
            f_out__info("[Uninit %02u/%02u] %s", p + 1, num, pack->name);
            pack->uninit();
        }
    }
}
