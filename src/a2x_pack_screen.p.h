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

extern SDL_Surface* a_screen;
extern Pixel* a_pixels;
extern int a_width;
extern int a_height;

#define A_SCREEN_SIZE (a_width * a_height * sizeof(Pixel))

#define a_screen_new()          malloc(A_SCREEN_SIZE)
#define a_screen_copy(dst, src) memcpy((dst), (src), A_SCREEN_SIZE)
extern void a_screen_copyPart(Pixel* dst, const int x, const int y, const int w, const int h);

#define a_screen_dup()                    \
({                                        \
    Pixel* const a__dst = a_screen_new(); \
    a_screen_copy(a__dst, a_pixels);      \
    a__dst;                               \
})

extern void a_screen_setTarget(Pixel* const p, const int w, const int h);
extern void a_screen_setTargetSprite(const Sprite* const s);
extern void a_screen_resetTarget(void);

extern void a_screen_show(void);
extern void a_screen_custom(void (*f)(void* const v), void* const v);

#endif // A2X_PACK_SCREEN_PH
