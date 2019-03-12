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

#include "a2x_system_includes.h"

typedef struct AScreen AScreen;

#include "a2x_pack_sprite.p.h"

extern APixel* a_screen_pixelsGetBuffer(void);

extern AVectorInt a_screen_sizeGet(void);
extern int a_screen_sizeGetWidth(void);
extern int a_screen_sizeGetHeight(void);

extern AScreen* a_screen_new(int Width, int Height);
extern AScreen* a_screen_dup(void);
extern void a_screen_free(AScreen* Screen);

extern void a_screen_copy(AScreen* Dst, const AScreen* Src);
extern void a_screen_blit(const AScreen* Screen);
extern void a_screen_clear(void);

extern void a_screen_targetPushScreen(const AScreen* Screen);
extern void a_screen_targetPushSprite(ASprite* Sprite);
extern void a_screen_targetPop(void);

extern void a_screen_clipSet(int X, int Y, int Width, int Height);
extern void a_screen_clipReset(void);

extern bool a_screen_boxOnScreen(int X, int Y, int W, int H);
extern bool a_screen_boxInsideScreen(int X, int Y, int W, int H);
extern bool a_screen_boxOnClip(int X, int Y, int W, int H);
extern bool a_screen_boxInsideClip(int X, int Y, int W, int H);
