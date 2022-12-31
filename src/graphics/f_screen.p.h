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

#ifndef F_INC_GRAPHICS_SCREEN_P_H
#define F_INC_GRAPHICS_SCREEN_P_H

#include "../general/f_system_includes.h"

#define F_SCREEN_RENDER_SOFTWARE 0
#define F_SCREEN_RENDER_SDL2 1

#include "../graphics/f_sprite.p.h"
#include "../math/f_vec.p.h"

extern FColorPixel* f_screen_pixelsGetBuffer(void);

extern FVecInt f_screen_sizeGet(void);
extern int f_screen_sizeGetWidth(void);
extern int f_screen_sizeGetHeight(void);

extern void f_screen_clear(void);

extern void f_screen_push(FSprite* Sprite, unsigned Frame);
extern void f_screen_pop(void);

extern void f_screen_clipSet(int X, int Y, int Width, int Height);
extern void f_screen_clipReset(void);

extern bool f_screen_boxOnScreen(int X, int Y, int Width, int Height);
extern bool f_screen_boxInsideScreen(int X, int Y, int Width, int Height);
extern bool f_screen_boxOnClip(int X, int Y, int Width, int Height);
extern bool f_screen_boxInsideClip(int X, int Y, int Width, int Height);

#endif // F_INC_GRAPHICS_SCREEN_P_H
