/*
    Copyright 2010, 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_PNG_V_H
#define F_INC_GRAPHICS_PNG_V_H

#include "f_png.p.h"

#include "../graphics/f_pixels.v.h"

extern FPixels* f_png__readFile(const char* Path);
extern FPixels* f_png__readMemory(const uint8_t* Data);
extern void f_png__write(const char* Path, const FPixels* Pixels, unsigned Frame, char* Title, char* Description);

#endif // F_INC_GRAPHICS_PNG_V_H
