/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_FPS_V_H
#define F_INC_GENERAL_FPS_V_H

#include "f_fps.p.h"

#include "../general/f_init.v.h"

extern const FPack f_pack__fps;

extern void f_fps__reset(void);
extern bool f_fps__tick(void);
extern void f_fps__frame(void);

#endif // F_INC_GENERAL_FPS_V_H
