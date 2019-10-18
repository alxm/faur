/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_GENERAL_CONSOLE_V_H
#define A_INC_GENERAL_CONSOLE_V_H

#include "general/a_console.p.h"

#include "general/a_main.v.h"
#include "general/a_out.v.h"

extern const APack a_pack__console;

extern void a_console__tick(void);
extern void a_console__draw(void);

extern bool a_console__isInitialized(void);
extern void a_console__write(AOutSource Source, AOutType Type, const char* Text);

#endif // A_INC_GENERAL_CONSOLE_V_H
