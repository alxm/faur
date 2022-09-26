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

#ifndef F_INC_PLATFORM_GRAPHICS_SDL_DRAW_V_H
#define F_INC_PLATFORM_GRAPHICS_SDL_DRAW_V_H

#include "f_sdl_draw.p.h"

extern void f_platform_api_sdl__drawSetColor(void);
extern void f_platform_api_sdl__drawSetBlend(void);

extern void f_platform_api_sdl__drawPixel(int X, int Y);
extern void f_platform_api_sdl__drawLine(int X1, int Y1, int X2, int Y2);
extern void f_platform_api_sdl__drawLineH(int X1, int X2, int Y);
extern void f_platform_api_sdl__drawLineV(int X, int Y1, int Y2);
extern void f_platform_api_sdl__drawRectangleOutline(int X, int Y, int Width, int Height);
extern void f_platform_api_sdl__drawRectangleFilled(int X, int Y, int Width, int Height);
extern void f_platform_api_sdl__drawCircleOutline(int X, int Y, int Radius);
extern void f_platform_api_sdl__drawCircleFilled(int X, int Y, int Radius);

#endif // F_INC_PLATFORM_GRAPHICS_SDL_DRAW_V_H
