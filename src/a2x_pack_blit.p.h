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

#ifndef A2X_PACK_BLIT_PH
#define A2X_PACK_BLIT_PH

#include "a2x_pack_screen.p.h"
#include "a2x_pack_sprite.p.h"

typedef enum {
    A_BLIT_PLAIN, A_BLIT_INVERSE,
    A_BLIT_RGB25, A_BLIT_RGB50, A_BLIT_RGB75, A_BLIT_RGB,
    A_BLIT_ALPHA, A_BLIT_SPRITEALPHA, A_BLIT_ARGB,
    A_BLIT_TYPE_NUM
} BlitType_t;

typedef enum {
    A_BLIT_NCNT, A_BLIT_NCT, A_BLIT_CNT, A_BLIT_CT, A_BLIT_CLIP_NUM
} BlitClip_t;

typedef void (*Blitter)(const Sprite* const s, const int x, const int y);

extern void a_blit_setType(BlitType_t t);
extern void a_blit_setClip(BlitClip_t c);
extern void a_blit_setAlpha(const uint8_t a);
extern void a_blit_setRGB(const uint8_t r, const uint8_t g, const uint8_t b);

extern Blitter a_blit;

extern void a_blit_c(const Sprite* const s);
extern void a_blit_ch(const Sprite* const s, const int y);
extern void a_blit_cv(const Sprite* const s, const int x);

#endif // A2X_PACK_BLIT_PH
