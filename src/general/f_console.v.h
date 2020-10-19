/*
    Copyright 2016-2018, 2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_CONSOLE_V_H
#define F_INC_GENERAL_CONSOLE_V_H

#include "f_console.p.h"

#include "../general/f_init.v.h"
#include "../general/f_out.v.h"

typedef struct {
    FOutSource source;
    FOutType type;
    char* text;
} FConsoleLine;

extern const FPack f_pack__console_0;
extern const FPack f_pack__console_1;

extern void f_console__tick(void);
extern void f_console__draw(void);

extern bool f_console__isInitialized(void);
extern void f_console__write(FOutSource Source, FOutType Type, const char* Text);

#endif // F_INC_GENERAL_CONSOLE_V_H
