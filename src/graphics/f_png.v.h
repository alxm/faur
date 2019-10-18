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

#ifndef A_INC_GRAPHICS_PNG_V_H
#define A_INC_GRAPHICS_PNG_V_H

#include "graphics/a_png.p.h"

#include "graphics/a_pixels.v.h"

extern APixels* a_png__readFile(const char* Path);
extern APixels* a_png__readMemory(const uint8_t* Data);
extern void a_png__write(const char* Path, const APixels* Pixels, unsigned Frame, char* Title, char* Description);

#endif // A_INC_GRAPHICS_PNG_V_H
