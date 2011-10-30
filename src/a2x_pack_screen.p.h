/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_SCREEN_PH
#define A2X_PACK_SCREEN_PH

#include "a2x_app_includes.h"

#include "a2x_pack_pixel.p.h"
#include "a2x_pack_sprite.p.h"

extern Pixel* a_pixels;
extern int a_width;
extern int a_height;

extern void a_screen_show(void);

extern Pixel* a_screen_dup(void);
extern Pixel* a_screen_new(void);
extern void a_screen_copy(Pixel* dst, const Pixel* src);
extern void a_screen_copyPart(Pixel* dst, int x, int y, int w, int h);

extern void a_screen_setTarget(Pixel* p, int w, int h);
extern void a_screen_setTargetSprite(const Sprite* s);
extern void a_screen_resetTarget(void);

#endif // A2X_PACK_SCREEN_PH
