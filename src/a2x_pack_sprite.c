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

static List* sprites;

void a_sprite__init(void)
{
    sprites = a_list_new();
}

void a_sprite__uninit(void)
{
    A_LIST_ITERATE(sprites, Sprite, s) {
        a_sprite__free(s);
    }

    a_list_free(sprites);
}

Sprite* a_sprite_fromFile(const char* path)
{
    int w = 0;
    int h = 0;
    Sprite* s;
    Pixel* pixels = NULL;

    a_png_readFile(path, &pixels, &w, &h);

    if(pixels == NULL) {
        return NULL;
    }

    s = a_sprite_fromPixels(pixels, w, h);
    free(pixels);

    return s;
}

Sprite* a_sprite_fromData(const uint8_t* data)
{
    int w;
    int h;
    Sprite* s;
    Pixel* pixels = NULL;

    a_png_readMemory(data, &pixels, &w, &h);

    if(pixels == NULL) {
        return NULL;
    }

    s = a_sprite_fromPixels(pixels, w, h);
    free(pixels);

    return s;
}

Sprite* a_sprite_fromPixels(Pixel* pixels, int w, int h)
{
    Sprite* s = a_sprite_blank(w, h);

    memcpy(s->data, pixels, w * h * sizeof(Pixel));
    a_sprite_refresh(s);

    return s;
}

Sprite* a_sprite_new(const Sprite* sheet, int x, int y)
{
    return a_sprite_zoomed(sheet, x, y, 1);
}

Sprite* a_sprite_zoomed(const Sprite* sheet, int x, int y, int zoom)
{
    int w = 0;
    int h = 0;

    const int width = sheet->w;
    const int height = sheet->h;

    for(int sheetx = x; sheetx < width; sheetx++) {
        const Pixel hPixel = a_sprite__getPixel(sheet, sheetx, y);

        // reached right edge
        if(hPixel == A_SPRITE_LIMIT || hPixel == A_SPRITE_END) {
            for(int sheety = y; sheety < height; sheety++) {
                const Pixel vPixel = a_sprite__getPixel(sheet, x, sheety);

                // reached bottom edge
                if(vPixel == A_SPRITE_LIMIT) {
                    w = sheetx - x;
                    h = sheety - y;
                    goto Done;
                }
            }
        }
    }

    Done:

    if(w == 0 || h == 0) {
        if(x == 0 && y == 0) {
            // no boundary borders for full-image sprites
            w = width;
            h = height;
        } else {
            a_out__error("Sprite coords %d, %d are invalid", x, y);
            return NULL;
        }
    }

    const int spritew = w * zoom;
    const int spriteh = h * zoom;

    Sprite* const s = a_sprite_blank(spritew, spriteh);

    const int wp = sheet->w;
    const Pixel* const src = sheet->data;
    Pixel* const dst = s->data;

    // put the pixels on

    for(int i = y; i < y + h; i++) {
        for(int j = x; j < x + w; j++) {
            const Pixel pixel = *(src + i * wp + j);

            const int dy = (i - y) * zoom;
            const int dx = (j - x) * zoom;

            for(int zy = zoom; zy--; ) {
                for(int zx = zoom; zx--; ) {
                    dst[(dy + zy) * spritew + dx + zx] = pixel;
                }
            }
        }
    }

    a_sprite_refresh(s);

    return s;
}

Sprite* a_sprite_blank(int w, int h)
{
    Sprite* const s = a_mem_malloc(sizeof(Sprite) + w * h * sizeof(Pixel));

    s->w = w;
    s->h = h;
    s->alpha = 255;
    s->spans = NULL;

    for(int i = w * h; i--; ) {
        s->data[i] = A_SPRITE_TRANSPARENT;
    }

    s->node = a_list_addLast(sprites, s);

    return s;
}

void a_sprite_free(Sprite* s)
{
    a_list_removeNode(s->node);
    a_sprite__free(s);
}

void a_sprite__free(Sprite* s)
{
    free(s->spans);
    free(s);
}

int a_sprite_w(const Sprite* s)
{
    return s->w;
}

int a_sprite_h(const Sprite* s)
{
    return s->h;
}

Pixel* a_sprite_data(Sprite* s)
{
    return s->data;
}

uint8_t a_sprite_getAlpha(const Sprite* s)
{
    return s->alpha;
}

void a_sprite_setAlpha(Sprite* s, uint8_t a)
{
    s->alpha = a;
}

Pixel a_sprite_getPixel(const Sprite* s, int x, int y)
{
    return *(s->data + y * s->w + x);
}

void a_sprite_refresh(Sprite* s)
{
    const int w = s->w;
    const int h = s->h;
    const Pixel* const dst = s->data;

    // Spans format:
    // [1 (draw) / 0 (transp)][[len]...][0 (end line)]

    unsigned int num = 0;
    const Pixel* dest = dst;

    for(int y = h; y--; ) {
        num += 3; // transparency start + first len + end line
        dest++; // start from the second pixel in the line
        for(int x = w - 1; x--; dest++) {
            if((*dest == A_SPRITE_TRANSPARENT && *(dest - 1) != A_SPRITE_TRANSPARENT)
                || (*dest != A_SPRITE_TRANSPARENT && *(dest - 1) == A_SPRITE_TRANSPARENT)) {
                num++; // transparency change, add a len
            }
        }
    }

    free(s->spans);
    s->spans = a_mem_malloc(num * sizeof(uint16_t));

    uint16_t* spans = s->spans;

    dest = dst;

    for(int y = h; y--; ) {
        *spans++ = *dest != A_SPRITE_TRANSPARENT; // transparency start
        dest++; // start from the second pixel in the line
        uint16_t len = 1;
        for(int x = 1; x < w; x++, dest++) {
            if((*dest == A_SPRITE_TRANSPARENT && *(dest - 1) != A_SPRITE_TRANSPARENT)
                || (*dest != A_SPRITE_TRANSPARENT && *(dest - 1) == A_SPRITE_TRANSPARENT)) {
                *spans++ = len; // record len
                len = 1;
            } else {
                len++;
            }
        }
        *spans++ = len; // record last len of line
        *spans++ = 0; // mark end of line
    }
}

Sprite* a_sprite_clone(const Sprite* src)
{
    Sprite* const s = a_sprite_blank(src->w, src->h);

    s->alpha = src->alpha;
    memcpy(s->data, src->data, src->w * src->h * sizeof(Pixel));
    a_sprite_refresh(s);

    return s;
}
