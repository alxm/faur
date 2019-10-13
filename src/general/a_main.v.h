/*
    Copyright 2010, 2016, 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_GENERAL_MAIN_V_H
#define A_INC_GENERAL_MAIN_V_H

#include "general/a_main.p.h"

#define A_PACK__PASSES_NUM 2

typedef void APackInit(void);
typedef void APackUninit(void);

typedef struct {
    const char* name;
    APackInit* init[A_PACK__PASSES_NUM];
    APackUninit* uninit[A_PACK__PASSES_NUM];
} APack;

extern void a__main(void);

extern void A__FATAL(const char* Format, ...) __attribute__((noreturn));

#endif // A_INC_GENERAL_MAIN_V_H
