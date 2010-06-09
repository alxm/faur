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

#include "a2x_pack_fix.p.h"
#include "a2x_pack_screen.p.h"
#include "a2x_pack_sprite.p.h"

typedef enum Blit_t {A_NCNT, A_NCT, A_CNT, A_CT} Blit_t;

/*
    Blitters
*/

#define a__blit_makeH2(area, type, params)     \
    extern void a_blit_##area##_##type params;

#define a__blit_makeH(type, params)    \
    a__blit_makeH2(NCNT, type, params) \
    a__blit_makeH2(NCT,  type, params) \
    a__blit_makeH2(CNT,  type, params) \
    a__blit_makeH2(CT,   type, params)

a__blit_makeH(plain,   (const Sprite* const s, const int x, const int y))
a__blit_makeH(rgb,     (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_makeH(inverse, (const Sprite* const s, const int x, const int y))
a__blit_makeH(a,       (const Sprite* const s, const int x, const int y, const fix8 a))
a__blit_makeH(alpha,   (const Sprite* const s, const int x, const int y))
a__blit_makeH(argb,    (const Sprite* const s, const int x, const int y, const fix8 a, const int r, const int g, const int b))
a__blit_makeH(a25rgb,  (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_makeH(a50rgb,  (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_makeH(a75rgb,  (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))

#define a_blit_NCNT a_blit_NCNT_plain
#define a_blit_NCT  a_blit_NCT_plain
#define a_blit_CNT  a_blit_CNT_plain
#define a_blit_CT   a_blit_CT_plain

#define a_blit_c (f, s)    (f)((s), (a_width - (s)->w) / 2, (a_height - (s)->h) / 2)
#define a_blit_ch(f, s, y) (f)((s), (a_width - (s)->w) / 2, (y))
#define a_blit_cv(f, s, x) (f)((s), (x),                    (a_height - (s)->h) / 2)

#endif // A2X_PACK_BLIT_PH
