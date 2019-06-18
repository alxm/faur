/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "general/a_system_includes.h"

#include "graphics/a_sprite.p.h"

extern APixel* a_screen_pixelsGetBuffer(void);

extern AVectorInt a_screen_sizeGet(void);
extern int a_screen_sizeGetWidth(void);
extern int a_screen_sizeGetHeight(void);

extern void a_screen_clear(void);

extern void a_screen_push(ASprite* Sprite, unsigned Frame);
extern void a_screen_pop(void);

extern void a_screen_clipSet(int X, int Y, int Width, int Height);
extern void a_screen_clipReset(void);

extern bool a_screen_boxOnScreen(int X, int Y, int W, int H);
extern bool a_screen_boxInsideScreen(int X, int Y, int W, int H);
extern bool a_screen_boxOnClip(int X, int Y, int W, int H);
extern bool a_screen_boxInsideClip(int X, int Y, int W, int H);
