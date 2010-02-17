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

#ifndef A2X_PACK_BLIT_H
#define A2X_PACK_BLIT_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "a2x_pack_fix.h"
#include "a2x_pack_list.h"
#include "a2x_pack_screen.h"
#include "a2x_pack_types.h"

typedef struct Sheet {
    int w;
    int h;
    Pixel* data;
    Pixel transparent;
    Pixel limit;
} Sheet;

typedef struct Sprite {
	int w;
	int h;
	Pixel* data;
	int*** spans;
    int* spansNum;
    fix8 alpha;
    Pixel t;
} Sprite;

typedef struct SpriteList {
    Sprite** sprites;
    fix8 num;
    fix8 current;
    fix8 speed;
    int dir;
} SpriteList;

typedef struct SpriteFrames {
    int numFrames;
    Sprite** frames;
} SpriteFrames;

typedef enum Blit_t {NCNT, NCT, CNT, CT, NCTinverse, CTinverse} Blit_t;

extern void (*a_blit_blitters[])(const Sprite* const s, const int x, const int y);

extern void a__blit_set(void);
extern void a__blit_free(void);

extern Sheet* a_blit_makeSheetFromFile(const char* const path);
extern Sheet* a_blit_makeSheet(Pixel* data, const int w, const int h);
extern Sheet* a_blit_makeSheet2(const Sheet* const sheet, const int x, const int y, const int w, const int h);
extern void a_blit_freeSheet(Sheet* const s);

#define a_blit_getPixel(s, x, y) (*((s)->data + (y) * (s)->w + (x)))

#define a_blit_makeSprite(g, x, y, w, h) a_blit_makeSpriteZoom((g), (x), (y), (w), (h), 1)
extern Sprite* a_blit_makeSpriteZoom(const Sheet* const graphic, const int x, const int y, const int w, const int h, const int zoom);
extern Sprite* a_blit_makeBlankSprite(const int w, const int h);
extern void a_blit_makeTransparent(Sprite* const s, const Sheet* const graphic);
extern void a_blit_freeSprite(Sprite* const s);

/*
    Blitters
*/

#define a__blit_makeH(type, params)    \
    a__blit_makeH2(NCNT, type, params) \
    a__blit_makeH2(NCT,  type, params) \
    a__blit_makeH2(CNT,  type, params) \
    a__blit_makeH2(CT,   type, params)

#define a__blit_makeH2(area, type, params)     \
    extern void a_blit_##area##_##type params;

a__blit_makeH(plain,   (const Sprite* const s, const int x, const int y))
a__blit_makeH(rgb,     (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_makeH(inverse, (const Sprite* const s, const int x, const int y))
a__blit_makeH(a,       (const Sprite* const s, const int x, const int y, const fix8 a))
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

/*
    Other stuff
*/

#define a_blit_putBackground(s) memcpy(a_pixels, (s)->data, A_SCREEN_SIZE)
#define a_blit_getBackground(s) memcpy((s)->data, a_pixels, A_SCREEN_SIZE)
extern void a_blit_getScreen(Pixel* dst, const Rect area);

extern SpriteList* a_blit_makeSpriteList(const int num, const int cycleLength);
extern void a_blit_freeSpriteList(SpriteList* const s);
extern void a_blit_addSprite(SpriteList* const s, Sprite* const sp);
extern Sprite* a_blit_nextSprite(SpriteList* const s);
extern void a_blit_changeDir(SpriteList* const s, const int dir);

extern SpriteFrames* a_blit_makeSpriteFrames(void);
extern void a_blit_freeSpriteFrames(SpriteFrames* const s);
extern void a_blit_addSpriteFrame(SpriteFrames* const s, Sprite* const f);

#endif // A2X_PACK_BLIT_H
