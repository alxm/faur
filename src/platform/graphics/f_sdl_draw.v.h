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

#include "../f_platform.v.h"

extern FCallApi_DrawSetColor f_platform_api_sdl__drawSetColor;
extern FCallApi_DrawSetBlend f_platform_api_sdl__drawSetBlend;

extern FCallApi_DrawPixel f_platform_api_sdl__drawPixel;
extern FCallApi_DrawLine f_platform_api_sdl__drawLine;
extern FCallApi_DrawLineH f_platform_api_sdl__drawLineH;
extern FCallApi_DrawLineV f_platform_api_sdl__drawLineV;
extern FCallApi_DrawRectangleOutline f_platform_api_sdl__drawRectangleOutline;
extern FCallApi_DrawRectangleFilled f_platform_api_sdl__drawRectangleFilled;
extern FCallApi_DrawCircleOutline f_platform_api_sdl__drawCircleOutline;
extern FCallApi_DrawCircleFilled f_platform_api_sdl__drawCircleFilled;

#endif // F_INC_PLATFORM_GRAPHICS_SDL_DRAW_V_H
