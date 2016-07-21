/*
    Copyright 2016 Alex Margarit

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

#pragma once

#include "a2x_pack_out.p.h"

#include "a2x_pack_console.v.h"
#include "a2x_pack_settings.v.h"

extern void a_out__init(void);
extern void a_out__uninit(void);

extern void a_out__message(char* Format, ...);
extern void a_out__warning(char* Format, ...);
extern void a_out__error(char* Format, ...);
extern void a_out__fatal(char* Format, ...);
extern void a_out__state(char* Format, ...);
