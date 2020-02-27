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

const char* f_init__app_name = F_CONFIG_APP_NAME;
const char* f_init__app_author = F_CONFIG_APP_AUTHOR;
char f_init__app_version[F_INIT__VERSION_LEN];

unsigned f_init__fps_tick = 30;
unsigned f_init__fps_draw = 30;

static const FPack* g_packs[] = {
    &f_pack__console,
    &f_pack__embed,
    &f_pack__platform,
    &f_pack__timer,
    &f_pack__input,
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
    &f_pack__ecs,
    &f_pack__fade,
    &f_pack__font,
};

void f_init__init(void)
{
    for(unsigned pass = 0; pass < F_PACK__PASSES_NUM; pass++) {
        for(unsigned pack = 0; pack < F_ARRAY_LEN(g_packs); pack++) {
            if(g_packs[pack]->init[pass]) {
                f_out__info("[%s] Init pass %d", g_packs[pack]->name, pass);
                g_packs[pack]->init[pass]();
            }
        }
    }
}

void f_init__uninit(void)
{
    for(unsigned pass = F_PACK__PASSES_NUM; pass--; ) {
        for(unsigned pack = F_ARRAY_LEN(g_packs); pack--; ) {
            if(g_packs[pack]->uninit[pass]) {
                f_out__info("[%s] Uninit pass %d", g_packs[pack]->name, pass);
                g_packs[pack]->uninit[pass]();
            }
        }
    }
}

void f_init_app(const char* Name, const char* Author, uint8_t VerMajor, uint8_t VerMinor, uint8_t VerMicro)
{
    f_init__app_name = Name;
    f_init__app_author = Author;

    f_str_fmt(f_init__app_version,
              F_INIT__VERSION_LEN,
              true,
              "%u.%u.%u",
              VerMajor,
              VerMinor,
              VerMicro);
}

void f_init_fps(unsigned TickRate, unsigned DrawRate)
{
    f_init__fps_tick = TickRate;
    f_init__fps_draw = DrawRate;
}
