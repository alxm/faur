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

#include "a2x_pack_sprite.v.h"

typedef struct Span {
    int x1;
    int x2;
} Span;

static List* sprites;

void a_sprite__set(void)
{
    sprites = a_list_set();
}

void a_sprite__free(void)
{
    while(a_list_iterate(sprites)) {
        a_sprite_free(a_list_current(sprites));
    }

    a_list_free(sprites);
}

Sprite* a_sprite_makeZoomed(const Sheet* const graphic, const int x, const int y, const int w, const int h, const int zoom)
{
    const int spritew = w * zoom;
    const int spriteh = h * zoom;

    Sprite* const s = a_sprite_makeBlank(spritew, spriteh);

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

    s->t = graphic->transparent;

    a_sprite_makeTransparent(s);

    return s;
}

Sprite* a_sprite_makeBlank(const int w, const int h)
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
        s->spans[i][0][2] = s->spans[i][0][1] - s->spans[i][0][0];
    }

    s->alpha = 255;
    s->t = DEFAULT_TRANSPARENT;

    a_list_addLast(sprites, s);

    return s;
}

void a_sprite_makeTransparent(Sprite* const s)
{
    const Pixel tpixel = s->t;

    const int spritew = s->w;
    const int spriteh = s->h;

    const Pixel* const dst = s->data;

    for(int i = spriteh; i--; ) {
        for(int j = s->spansNum[i]; j--; ) {
            free(s->spans[i][j]);
        }

        free(s->spans[i]);
    }

    for(int i = 0; i < spriteh; i++) {
        List* const spans = a_list_set();

        int start = 0;
        int transparent = 1;

        for(int j = 0; j < spritew; j++) {
            const Pixel pixel = dst[i * spritew + j];

            int oldTra = transparent;
            transparent = pixel == tpixel;

            if(oldTra && !transparent) {
                if(j == spritew - 1) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = j;
                    span->x2 = spritew;

                    a_list_addLast(spans, span);
                } else {
                    start = j;
                }
            } else if(!oldTra) {
                if(j == spritew - 1) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = start;
                    span->x2 = spritew - transparent;

                    a_list_addLast(spans, span);
                } else if(transparent) {
                    Span* const span = malloc(sizeof(Span));

                    span->x1 = start;
                    span->x2 = j;

                    a_list_addLast(spans, span);
                }
            }
        }

        s->spansNum[i] = a_list_size(spans);
        s->spans[i] = malloc(s->spansNum[i] * sizeof(int*));

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

void a_sprite_free(Sprite* const s)
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

    a_list_remove(sprites, s);
}

Sprite* a_sprite_clone(const Sprite* const src)
{
    Sprite* const s = malloc(sizeof(Sprite));

    s->w = src->w;
    s->h = src->h;

    s->data = malloc(s->w * s->h * sizeof(Pixel));
    memcpy(s->data, src->data, s->w * s->h * sizeof(Pixel));

    s->spans = malloc(s->h * sizeof(int**));
    s->spansNum = malloc(s->h * sizeof(int));

    for(int i = s->h; i--; ) {
        s->spansNum[i] = src->spansNum[i];
        s->spans[i] = malloc(s->spansNum[i] * sizeof(int*));

        for(int j = s->spansNum[i]; j--; ) {
            s->spans[i][j] = malloc(3 * sizeof(int));

            for(int k = 3; k--; ) {
                s->spans[i][j][k] = src->spans[i][j][k];
            }
        }
    }

    s->alpha = src->alpha;
    s->t = src->t;

    a_list_addLast(sprites, s);

    return s;
}

int a_sprite_w(const Sprite* const s)
{
    return s->w;
}

int a_sprite_h(const Sprite* const s)
{
    return s->h;
}

Pixel* a_sprite_data(const Sprite* const s)
{
    return s->data;
}

uint8_t a_sprite_getAlpha(const Sprite* const s)
{
    return s->alpha;
}

void a_sprite_setAlpha(Sprite* const s, const uint8_t a)
{
    s->alpha = a;
}

Pixel a_sprite_getTransparent(const Sprite* const s)
{
    return s->t;
}

void a_sprite_setTransparent(Sprite* const s, const Pixel c)
{
    s->t = c;
}

Pixel a_sprite_getPixel(const Sprite* const s, const int x, const int y)
{
    return *(s->data + y * s->w + x);
}
