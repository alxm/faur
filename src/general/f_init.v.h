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

#ifndef F_INC_GENERAL_INIT_V_H
#define F_INC_GENERAL_INIT_V_H

#include "f_init.p.h"

#define F_PACK__PASSES_NUM 2
#define F_INIT__VERSION_LEN (sizeof("255.255.255"))

typedef void FPackInit(void);
typedef void FPackUninit(void);

typedef struct {
    const char* name;
    FPackInit* init[F_PACK__PASSES_NUM];
    FPackUninit* uninit[F_PACK__PASSES_NUM];
} FPack;

extern const char* f_init__app_name;
extern const char* f_init__app_author;
extern char f_init__app_version[F_INIT__VERSION_LEN];
extern unsigned f_init__fps_tick, f_init__fps_draw;
extern unsigned f_init__ecs_com, f_init__ecs_sys;

extern void f_init__init(void);
extern void f_init__uninit(void);

#endif // F_INC_GENERAL_INIT_V_H
