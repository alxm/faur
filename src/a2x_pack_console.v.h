/*
    Copyright 2016-2018 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#pragma once

#include "a2x_pack_console.p.h"

#include "a2x_pack_out.v.h"

extern void a_console__init(void);
extern void a_console__init2(void);
extern void a_console__uninit(void);

extern void a_console__tick(void);
extern void a_console__draw(void);

extern bool a_console__isInitialized(void);
extern void a_console__write(AOutSource Source, AOutType Type, const char* Text, bool Overwrite);
