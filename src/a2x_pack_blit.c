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

#include "a2x_pack_blit.h"

static List* a__spriteList;

void (*a_blit_blitters[])(const Sprite* const s, const int x, const int y) = {
    &a_blit_NCNT, &a_blit_NCT, &a_blit_CNT, &a_blit_CT, &a_blit_NCT_inverse, &a_blit_CT_inverse
};

void a__blit_set(void)
{
    a__spriteList = a_list_set();
}

void a__blit_free(void)
{
    while(a_list_iterate(a__spriteList)) {
        a_blit_freeSprite(a_list_current(a__spriteList));
    }

    a_list_free(a__spriteList);
}

static Uint32 a__blit_getPixel(const SDL_Surface* const s, const int x, const int y)
{
    const Uint8* const p = (Uint8*)s->pixels + y * s->pitch + x * s->format->BytesPerPixel;

    switch(s->format->BytesPerPixel) {
        case 1: {
            return *p;
        } break;

        case 2: {
            return *(Uint16*)p;
        } break;

        case 3: {
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                return (p[0] << 16) | (p[1] << 8) | (p[2]);
            } else {
                return (p[0]) | (p[1] << 8) | (p[2] << 16);
            }
        } break;

        case 4: {
            return *(Uint32*)p;
        } break;

        default: {
            return 0;
        } break;
    }
}

static void a__blit_sheetValues(Sheet* const s)
{
    s->transparent = a_blit_getPixel(s, s->w - 1, s->h - 1);
    s->limit = a_blit_getPixel(s, s->w - 2, s->h - 1);
}

Sheet* a_blit_makeSheetFromFile(const char* const path)
{
    SDL_Surface* const surf = IMG_Load(path);

    Sheet* const s = malloc(sizeof(Sheet));

    s->w = surf->w;
    s->h = surf->h;
    s->data = malloc(s->w * s->h * sizeof(Pixel));

    for(int i = 0; i < s->h; i++) {
        for(int j = 0; j < s->w; j++) {
            Uint8 r, g, b;
            SDL_GetRGB(a__blit_getPixel(surf, j, i), surf->format, &r, &g, &b);

            s->data[i * s->w + j] = a_screen_makePixel(r, g, b);
        }
    }

    SDL_FreeSurface(surf);

    a__blit_sheetValues(s);

    return s;
}

Sheet* a_blit_makeSheet(Pixel* data, const int w, const int h)
{
    Sheet* const s = malloc(sizeof(Sheet));

    s->w = w;
    s->h = h;
    s->data = a_mem_decodeRLE(data, w * h, sizeof(Pixel), NULL);

    a__blit_sheetValues(s);

    return s;
}

Sheet* a_blit_makeSheet2(const Sheet* const sheet, const int x, const int y, const int w, const int h)
{
    Sheet* const s = malloc(sizeof(Sheet));

    s->w = w;
    s->h = h;
    s->data = malloc(w * h * sizeof(Pixel));

    const int W = sheet->w;
    const Pixel* const src = sheet->data;

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            s->data[i * w + j] = *(src + (i + y) * W + (j + x));
        }
    }

    s->transparent = sheet->transparent;
    s->limit = sheet->limit;

    return s;
}

void a_blit_freeSheet(Sheet* const s)
{
    free(s->data);
    free(s);
}

Sprite* a_blit_makeSpriteZoom(const Sheet* const graphic, const int x, const int y, const int w, const int h, const int zoom)
{
    const int spritew = w * zoom;
    const int spriteh = h * zoom;

    Sprite* const s = malloc(sizeof(Sprite));

    s->w = spritew;
    s->h = spriteh;
    s->data = malloc(spritew * spriteh * sizeof(Pixel));

    const int wp = graphic->w;
    const Pixel* const data = graphic->data;
    Pixel* const dst = s->data;

    // put the pixels on

    for(int i = y; i < y + h; i++) {
        for(int j = x; j < x + w; j++) {
            const Pixel pixel = *(data + i * wp + j);

            const int dy = (i - y) * zoom;
            const int dx = (j - x) * zoom;

            for(int zy = zoom; zy--; ) {
                for(int zx = zoom; zx--; ) {
                    dst[(dy + zy) * spritew + dx + zx] = pixel;
                }
            }
        }
    }

    a_blit_makeTransparent(s, graphic);

    a_list_addLast(a__spriteList, s);

    return s;
}

Sprite* a_blit_makeBlankSprite(const int w, const int h)
{
    Sprite* const s = malloc(sizeof(Sprite));

    s->w = w;
    s->h = h;
    s->data = malloc(w * h * sizeof(Pixel));
    s->spansNum = malloc(h * sizeof(int));
    s->spans = malloc(h * sizeof(int**));

    for(int i = 0; i < h; i++) {
        s->spansNum[i] = 1;
        s->spans[i] = malloc(1 * sizeof(int*));

        s->spans[i][0] = malloc(3 * sizeof(int));

        s->spans[i][0][0] = 0;
        s->spans[i][0][1] = w;
        s->spans[i][0][2] = (s->spans[i][0][1] - s->spans[i][0][0]) << 1;
    }

    a_list_addLast(a__spriteList, s);

    return s;
}

void a_blit_makeTransparent(Sprite* const s, const Sheet* const graphic)
{
    const Pixel tpixel = graphic->transparent;

    s->t = graphic->transparent;

    const int spritew = s->w;
    const int spriteh = s->h;

    const int verti = spriteh;
    const int horiz = spritew;

    s->spans = malloc(verti * sizeof(int**));
    s->spansNum = malloc(verti * sizeof(int));

    const Pixel* const dst = s->data;

    for(int i = 0; i < verti; i++) {
        List* const spans = a_list_set();

        int start = 0;
        int transparent = 1;

        for(int j = 0; j < horiz; j++) {
            const Pixel pixel = dst[i * horiz + j];

            int oldTra = transparent;
            transparent = pixel == tpixel;

            if(oldTra && !transparent) {
                if(j == horiz - 1) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = j;
                    span->x2 = horiz;

                    a_list_addLast(spans, span);
                } else {
                    start = j;
                }
            } else if(!oldTra) {
                if(j == horiz - 1) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = start;
                    span->x2 = horiz - transparent;

                    a_list_addLast(spans, span);
                } else if(transparent) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = start;
                    span->x2 = j;

                    a_list_addLast(spans, span);
                }
            }
        }

        s->spansNum[i] = spans->items;
        s->spans[i] = malloc(spans->items * sizeof(int*));

        int counter = 0;

        while(a_list_iterate(spans)) {
            const int x1 = ((Span*)a_list_current(spans))->x1;
            const int x2 = ((Span*)a_list_current(spans))->x2;

            s->spans[i][counter] = malloc(3 * sizeof(int));

            s->spans[i][counter][0] = x1;
            s->spans[i][counter][1] = x2;
            s->spans[i][counter][2] = x2 - x1;

            counter++;
        }

        a_list_freeContent(spans);
    }
}

void a_blit_freeSprite(Sprite* const s)
{
    free(s->data);

    for(int i = 0; i < s->h; i++) {
        for(int j = 0; j < s->spansNum[i]; j++) {
            free(s->spans[i][j]);
        }
        free(s->spans[i]);
    }
    free(s->spans);
    free(s->spansNum);

    free(s);
}

/*
    Blit area
*/

// no clip, no transparent

#define NCNT(e)  \
{                \
    NCNT_START { \
        {e}      \
    } NCNT_END;  \
}

#define NCNT_START                           \
    const int w = s->w;                      \
    const int h = s->h;                      \
    const int diff = a_width - w;            \
                                             \
    const Pixel* src = s->data;              \
    Pixel* dst = a_pixels + y * a_width + x; \
                                             \
    for(int i = h; i--; dst += diff) {       \
        for(int j = w; j--; dst++, src++) {

#define NCNT_END \
        }        \
    }

// no clip, transparent

#define NCT(e)  \
{               \
    NCT_START { \
        {e}     \
    } NCT_END;  \
}

#define NCT_START                                               \
    const int w = s->w;                                         \
    const int h = s->h;                                         \
                                                                \
    const Pixel* src2 = s->data;                                \
    Pixel* dst2 = a_pixels + y * a_width + x;                   \
                                                                \
    for(int i = 0; i < h; i++, dst2 += a_width, src2 += w) {    \
        const int spansNum = s->spansNum[i];                    \
                                                                \
        for(int j = 0; j < spansNum; j++) {                     \
            const Pixel* src = src2 + s->spans[i][j][0];        \
            Pixel* dst = dst2 + s->spans[i][j][0];              \
                                                                \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) { \

#define NCT_END \
            }   \
        }       \
    }

// clip, no transparent

#define CNT(e)  \
{               \
    CNT_START { \
        {e}     \
    } CNT_END;  \
}

#define CNT_START                                                         \
    const int w = s->w;                                                   \
    const int h = s->h;                                                   \
                                                                          \
    if(y + h <= 0 || y >= a_height || x + w <= 0 || x >= a_width) return; \
                                                                          \
    const int yTclip = (y < 0) ? (-y) : 0;                                \
    const int yBclip = (y + h > a_height) ? (y + h - a_height) : 0;       \
    const int xLclip = (x < 0) ? (-x) : 0;                                \
    const int xRclip = (x + w > a_width) ? (x + w - a_width) : 0;         \
                                                                          \
    const int W = (w - xLclip - xRclip) << 1;                             \
    const int H = h - yTclip - yBclip;                                    \
                                                                          \
    const Pixel* src2 = s->data + yTclip * w + xLclip;                    \
    Pixel* dst2 = a_pixels + (y + yTclip) * a_width + x + xLclip;         \
                                                                          \
    for(int i = 0; i < H; i++, dst2 += a_width, src2 += w) {              \
        const Pixel* src = src2;                                          \
        Pixel* dst = dst2;                                                \
                                                                          \
        for(int j = W; j--; dst++, src++) {                               \

#define CNT_END \
        }       \
    }

// clip, transparent

#define CT(e)             \
{                         \
    CT_START {            \
        CT_LEFT_START {   \
            {e}           \
        } CT_LEFT_END;    \
                          \
        CT_MIDDLE_START { \
            {e}           \
        } CT_MIDDLE_END;  \
                          \
        CT_RIGHT_START {  \
            {e}           \
        } CT_RIGHT_END;   \
    } CT_END;             \
}

#define CT_START                                                               \
    const int w = s->w;                                                        \
    const int h = s->h;                                                        \
                                                                               \
    if(y + h <= 0 || y >= a_height || x + w <= 0 || x >= a_width) return;      \
                                                                               \
    const int yTclip = (y < 0) ? (-y) : 0;                                     \
    const int xLclip = (x < 0) ? (-x) : 0;                                     \
                                                                               \
    const int H = h - ((y + h > a_height) ? (y + h - a_height) : 0);           \
                                                                               \
    const Pixel* src2 = s->data + yTclip * w;                                  \
    Pixel* dst2 = a_pixels + (y + yTclip) * a_width + x;                       \
                                                                               \
    for(int i = yTclip; i < H; i++, dst2 += a_width, src2 += w) {              \
        int j;                                                                 \
        const int spansNum = s->spansNum[i];                                   \
                                                                               \
        for(j = 0; j < spansNum && s->spans[i][j][1] <= xLclip; j++) continue; \

#define CT_LEFT_START                                                    \
        if(j < spansNum && s->spans[i][j][0] < xLclip) {                 \
            const Pixel* src = src2 + xLclip;                            \
            Pixel* dst = dst2 + xLclip;                                  \
                                                                         \
            for(int k = s->spans[i][j][1] - xLclip; k--; dst++, src++) { \

#define CT_LEFT_END \
            }       \
                    \
            j++;    \
        }

#define CT_MIDDLE_START                                                 \
        for( ; j < spansNum && x + s->spans[i][j][1] <= a_width; j++) { \
            const Pixel* src = src2 + s->spans[i][j][0];                \
            Pixel* dst = dst2 + s->spans[i][j][0];                      \
                                                                        \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) {

#define CT_MIDDLE_END \
            }         \
        }

#define CT_RIGHT_START                                                          \
        if(j < spansNum && x + s->spans[i][j][0] < a_width) {                   \
            const Pixel* src = src2 + s->spans[i][j][0];                        \
            Pixel* dst = dst2 + s->spans[i][j][0];                              \
                                                                                \
            for(int k = a_width - (x + s->spans[i][j][0]); k--; dst++, src++) {

#define CT_RIGHT_END \
            }        \
        }

#define CT_END \
    }

/*
    Blit type
*/

#define BLIT_plain_setup

#define BLIT_plain_do \
    *dst = *src;

#define BLIT_rgb_setup                               \
    const Pixel color = a_screen_makePixel(r, g, b);

#define BLIT_rgb_do \
    *dst = color;

#define BLIT_inverse_setup

#define BLIT_inverse_do \
    *dst = ~*dst;

#define BLIT_a_setup

#define BLIT_a_do                                             \
    const Pixel cd = *dst;                                    \
    const Pixel cs = *src;                                    \
                                                              \
    const int R = a_screen_pixelRed(cd);                      \
    const int G = a_screen_pixelGreen(cd);                    \
    const int B = a_screen_pixelBlue(cd);                     \
                                                              \
    *dst = a_screen_makePixel(                                \
        R + a_fix8_fixtoi((a_screen_pixelRed(cs) - R) * a),   \
        G + a_fix8_fixtoi((a_screen_pixelGreen(cs) - G) * a), \
        B + a_fix8_fixtoi((a_screen_pixelBlue(cs) - B) * a)   \
    );

#define BLIT_argb_setup

#define BLIT_argb_do                       \
    const Pixel cd = *dst;                 \
                                           \
    const int R = a_screen_pixelRed(cd);   \
    const int G = a_screen_pixelGreen(cd); \
    const int B = a_screen_pixelBlue(cd);  \
                                           \
    *dst = a_screen_makePixel(             \
        R + a_fix8_fixtoi((r - R) * a),    \
        G + a_fix8_fixtoi((g - G) * a),    \
        B + a_fix8_fixtoi((b - B) * a)     \
    );

#define BLIT_a25rgb_setup

#define BLIT_a25rgb_do                     \
    const Pixel cd = *dst;                 \
                                           \
    const int R = a_screen_pixelRed(cd);   \
    const int G = a_screen_pixelGreen(cd); \
    const int B = a_screen_pixelBlue(cd);  \
                                           \
    *dst = a_screen_makePixel(             \
        (R >> 1) + ((R + r) >> 2),         \
        (G >> 1) + ((G + g) >> 2),         \
        (B >> 1) + ((B + b) >> 2)          \
    );

#define BLIT_a50rgb_setup

#define BLIT_a50rgb_do                     \
    const Pixel cd = *dst;                 \
                                           \
    const int R = a_screen_pixelRed(cd);   \
    const int G = a_screen_pixelGreen(cd); \
    const int B = a_screen_pixelBlue(cd);  \
                                           \
    *dst = a_screen_makePixel(             \
        (R + r) >> 1,                      \
        (G + g) >> 1,                      \
        (B + b) >> 1                       \
    );

#define BLIT_a75rgb_setup

#define BLIT_a75rgb_do                     \
    const Pixel cd = *dst;                 \
                                           \
    const int R = a_screen_pixelRed(cd);   \
    const int G = a_screen_pixelGreen(cd); \
    const int B = a_screen_pixelBlue(cd);  \
                                           \
    *dst = a_screen_makePixel(             \
        (R >> 2) + (r >> 2) + (r >> 1),    \
        (G >> 2) + (g >> 2) + (g >> 1),    \
        (B >> 2) + (b >> 2) + (b >> 1)     \
    );

/*
    Blitters
*/

#define a__blit_make(type, params)    \
    a__blit_make2(NCNT, type, params) \
    a__blit_make2(NCT,  type, params) \
    a__blit_make2(CNT,  type, params) \
    a__blit_make2(CT,   type, params)

#define a__blit_make2(area, type, params) \
    void a_blit_##area##_##type params    \
    {                                     \
        BLIT_##type##_setup               \
        area(BLIT_##type##_do)            \
    }

a__blit_make(plain, (const Sprite* const s, const int x, const int y))
a__blit_make(rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(inverse, (const Sprite* const s, const int x, const int y))
a__blit_make(a, (const Sprite* const s, const int x, const int y, const fix8 a))
a__blit_make(argb, (const Sprite* const s, const int x, const int y, const fix8 a, const int r, const int g, const int b))
a__blit_make(a25rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(a50rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(a75rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))

/*
    Other stuff
*/

void a_blit_getScreen(Pixel* dst, const Rect area)
{
    const Pixel* screen = a_pixels + area.y * a_width + area.x;
    const int w = area.w;

    for(int i = area.h; i--; ) {
        memcpy(dst, screen, w * sizeof(Pixel));
        dst += w;
        screen += a_width;
    }
}

SpriteList* a_blit_makeSpriteList(const int num, const int cycleLength)
{
    SpriteList* const s = malloc(sizeof(SpriteList));

    s->sprites = malloc(num * sizeof(Sprite*));
    s->num = a_fix8_itofix(num);
    s->current = 0;
    s->speed = a_fix8_itofix(num) / cycleLength;
    s->dir = 1;

    return s;
}

void a_blit_freeSpriteList(SpriteList* const s)
{
    free(s->sprites);
    free(s);
}

void a_blit_addSprite(SpriteList* const s, Sprite* const sp)
{
    s->sprites[a_fix8_fixtoi(s->current)] = sp;

    s->current += FONE8;
    if(s->current == s->num) s->current = 0;
}

Sprite* a_blit_nextSprite(SpriteList* const s)
{
    Sprite* const sp = s->sprites[a_fix8_fixtoi(s->current)];

    s->current += s->speed * s->dir;
    if(s->current >= s->num) s->current = 0;
    else if(s->current < 0) s->current = s->num - FONE8;

    return sp;
}

void a_blit_changeDir(SpriteList* const s, const int dir)
{
    s->dir = dir;
}

SpriteFrames* a_blit_makeSpriteFrames(void)
{
    SpriteFrames* const s = malloc(sizeof(SpriteFrames));

    s->numFrames = 0;
    s->frames = NULL;

    return s;
}

void a_blit_freeSpriteFrames(SpriteFrames* const s)
{
    free(s->frames);
    free(s);
}

void a_blit_addSpriteFrame(SpriteFrames* const s, Sprite* const f)
{
    Sprite** const tempFrames = s->frames;
    s->frames = malloc(++(s->numFrames) * sizeof(Sprite*));

    for(int i = s->numFrames - 1; i--; ) {
        s->frames[i] = tempFrames[i];
    }

    s->frames[s->numFrames - 1] = f;
    free(tempFrames);
}
