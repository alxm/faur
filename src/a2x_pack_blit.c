/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_blit.p.h"
#include "a2x_pack_blit.v.h"

void (*a_blit_blitters[])(const Sprite* const s, const int x, const int y) = {
    &a_blit_NCNT, &a_blit_NCT, &a_blit_CNT, &a_blit_CT
};

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

#define NCNT_START                               \
    const int screenWidth = a_width;             \
                                                 \
    const int w = s->w;                          \
    const int h = s->h;                          \
    const int diff = screenWidth - w;            \
                                                 \
    const Pixel* src = s->data;                  \
    Pixel* dst = a_pixels + y * screenWidth + x; \
                                                 \
    for(int i = h; i--; dst += diff) {           \
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

#define NCT_START                                                \
    const int screenWidth = a_width;                             \
                                                                 \
    const int w = s->w;                                          \
    const int h = s->h;                                          \
                                                                 \
    const Pixel* src2 = s->data;                                 \
    Pixel* dst2 = a_pixels + y * screenWidth + x;                \
                                                                 \
    for(int i = 0; i < h; i++, dst2 += screenWidth, src2 += w) { \
        const int spansNum = s->spansNum[i];                     \
                                                                 \
        for(int j = 0; j < spansNum; j++) {                      \
            const Pixel* src = src2 + s->spans[i][j][0];         \
            Pixel* dst = dst2 + s->spans[i][j][0];               \
                                                                 \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) {

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

#define CNT_START                                                           \
    const int screenWidth = a_width;                                        \
    const int screenHeight = a_height;                                      \
                                                                            \
    const int w = s->w;                                                     \
    const int h = s->h;                                                     \
                                                                            \
    if(y + h <= 0 || y >= screenHeight || x + w <= 0 || x >= screenWidth) { \
        return;                                                             \
    }                                                                       \
                                                                            \
    const int yTclip = (y < 0) ? (-y) : 0;                                  \
    const int yBclip = a_math_max(0, y + h - screenHeight);                 \
    const int xLclip = (x < 0) ? (-x) : 0;                                  \
    const int xRclip = a_math_max(0, x + w - screenWidth);                  \
                                                                            \
    const int W = (w - xLclip - xRclip);                                    \
    const int H = h - yTclip - yBclip;                                      \
                                                                            \
    const Pixel* src2 = s->data + yTclip * w + xLclip;                      \
    Pixel* dst2 = a_pixels + (y + yTclip) * screenWidth + x + xLclip;       \
                                                                            \
    for(int i = 0; i < H; i++, dst2 += screenWidth, src2 += w) {            \
        const Pixel* src = src2;                                            \
        Pixel* dst = dst2;                                                  \
                                                                            \
        for(int j = W; j--; dst++, src++) {

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

#define CT_START                                                            \
    const int screenWidth = a_width;                                        \
    const int screenHeight = a_height;                                      \
                                                                            \
    const int w = s->w;                                                     \
    const int h = s->h;                                                     \
                                                                            \
    if(y + h <= 0 || y >= screenHeight || x + w <= 0 || x >= screenWidth) { \
        return;                                                             \
    }                                                                       \
                                                                            \
    const int yTclip = (y < 0) ? (-y) : 0;                                  \
    const int xLclip = (x < 0) ? (-x) : 0;                                  \
                                                                            \
    const int H = h - a_math_max(0, y + h - screenHeight);                  \
                                                                            \
    const Pixel* src2 = s->data + yTclip * w;                               \
    Pixel* dst2 = a_pixels + (y + yTclip) * screenWidth + x;                \
                                                                            \
    for(int i = yTclip; i < H; i++, dst2 += screenWidth, src2 += w) {       \
        int j;                                                              \
        const int spansNum = s->spansNum[i];                                \
                                                                            \
        for(j = 0; j < spansNum && s->spans[i][j][1] <= xLclip; j++) {      \
            continue;                                                       \
        }

#define CT_LEFT_START                                                                             \
        if(j < spansNum && s->spans[i][j][0] < xLclip) {                                          \
            const Pixel* src = src2 + xLclip;                                                     \
            Pixel* dst = dst2 + xLclip;                                                           \
                                                                                                  \
            for(int k = a_math_min(s->spans[i][j][1] - xLclip, screenWidth); k--; dst++, src++) {

#define CT_LEFT_END \
            }       \
                    \
            j++;    \
        }

#define CT_MIDDLE_START                                                     \
        for( ; j < spansNum && x + s->spans[i][j][1] <= screenWidth; j++) { \
            const Pixel* src = src2 + s->spans[i][j][0];                    \
            Pixel* dst = dst2 + s->spans[i][j][0];                          \
                                                                            \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) {

#define CT_MIDDLE_END \
            }         \
        }

#define CT_RIGHT_START                                                              \
        if(j < spansNum && x + s->spans[i][j][0] < screenWidth) {                   \
            const Pixel* src = src2 + s->spans[i][j][0];                            \
            Pixel* dst = dst2 + s->spans[i][j][0];                                  \
                                                                                    \
            for(int k = screenWidth - (x + s->spans[i][j][0]); k--; dst++, src++) {

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

#define BLIT_rgb_setup                         \
    const Pixel color = a_pixel_make(r, g, b);

#define BLIT_rgb_do \
    *dst = color;

#define BLIT_inverse_setup

#define BLIT_inverse_do \
    *dst = ~*dst;

#define BLIT_a_setup

#define BLIT_a_do                                       \
    const Pixel cd = *dst;                              \
    const Pixel cs = *src;                              \
                                                        \
    const int R = a_pixel_red(cd);                      \
    const int G = a_pixel_green(cd);                    \
    const int B = a_pixel_blue(cd);                     \
                                                        \
    *dst = a_pixel_make(                                \
        R + (((a_pixel_red(cs) - R) * a) >> 8),         \
        G + (((a_pixel_green(cs) - G) * a) >> 8),       \
        B + (((a_pixel_blue(cs) - B) * a) >> 8)         \
    );

#define BLIT_alpha_setup     \
    const fix8 a = s->alpha;

#define BLIT_alpha_do                                   \
    const Pixel cd = *dst;                              \
    const Pixel cs = *src;                              \
                                                        \
    const int R = a_pixel_red(cd);                      \
    const int G = a_pixel_green(cd);                    \
    const int B = a_pixel_blue(cd);                     \
                                                        \
    *dst = a_pixel_make(                                \
        R + (((a_pixel_red(cs) - R) * a) >> 8),         \
        G + (((a_pixel_green(cs) - G) * a) >> 8),       \
        B + (((a_pixel_blue(cs) - B) * a) >> 8)         \
    );

#define BLIT_argb_setup

#define BLIT_argb_do                    \
    const Pixel cd = *dst;              \
                                        \
    const int R = a_pixel_red(cd);      \
    const int G = a_pixel_green(cd);    \
    const int B = a_pixel_blue(cd);     \
                                        \
    *dst = a_pixel_make(                \
        R + (((r - R) * a) >> 8),       \
        G + (((g - G) * a) >> 8),       \
        B + (((b - B) * a) >> 8)        \
    );

#define BLIT_a25rgb_setup

#define BLIT_a25rgb_do               \
    const Pixel cd = *dst;           \
                                     \
    const int R = a_pixel_red(cd);   \
    const int G = a_pixel_green(cd); \
    const int B = a_pixel_blue(cd);  \
                                     \
    *dst = a_pixel_make(             \
        (R >> 1) + ((R + r) >> 2),   \
        (G >> 1) + ((G + g) >> 2),   \
        (B >> 1) + ((B + b) >> 2)    \
    );

#define BLIT_a50rgb_setup

#define BLIT_a50rgb_do               \
    const Pixel cd = *dst;           \
                                     \
    const int R = a_pixel_red(cd);   \
    const int G = a_pixel_green(cd); \
    const int B = a_pixel_blue(cd);  \
                                     \
    *dst = a_pixel_make(             \
        (R + r) >> 1,                \
        (G + g) >> 1,                \
        (B + b) >> 1                 \
    );

#define BLIT_a75rgb_setup

#define BLIT_a75rgb_do                  \
    const Pixel cd = *dst;              \
                                        \
    const int R = a_pixel_red(cd);      \
    const int G = a_pixel_green(cd);    \
    const int B = a_pixel_blue(cd);     \
                                        \
    *dst = a_pixel_make(                \
        (R >> 2) + (r >> 2) + (r >> 1), \
        (G >> 2) + (g >> 2) + (g >> 1), \
        (B >> 2) + (b >> 2) + (b >> 1)  \
    );

/*
    Blitters
*/

#define a__blit_make2(area, type, params) \
    void a_blit_##area##_##type params    \
    {                                     \
        BLIT_##type##_setup               \
        area(BLIT_##type##_do)            \
    }

#define a__blit_make(type, params)    \
    a__blit_make2(NCNT, type, params) \
    a__blit_make2(NCT,  type, params) \
    a__blit_make2(CNT,  type, params) \
    a__blit_make2(CT,   type, params)

a__blit_make(plain, (const Sprite* const s, const int x, const int y))
a__blit_make(rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(inverse, (const Sprite* const s, const int x, const int y))
a__blit_make(a, (const Sprite* const s, const int x, const int y, const fix8 a))
a__blit_make(alpha, (const Sprite* const s, const int x, const int y))
a__blit_make(argb, (const Sprite* const s, const int x, const int y, const fix8 a, const int r, const int g, const int b))
a__blit_make(a25rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(a50rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
a__blit_make(a75rgb, (const Sprite* const s, const int x, const int y, const int r, const int g, const int b))
