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

#include "a2x_pack_sheet.p.h"
#include "a2x_pack_sheet.v.h"

#define DEFAULT_TRANSPARENT a_screen_makePixel(255, 0, 255)
#define DEFAULT_LIMIT       a_screen_makePixel(0, 255, 0)
#define DEFAULT_END         a_screen_makePixel(0, 255, 255)

Sheet* a_sheet_fromFile(const char* const path)
{
    SDL_Surface* const sf = IMG_Load(path);
    Sheet* const s = malloc(sizeof(Sheet));

    s->w = sf->w;
    s->h = sf->h;
    s->data = malloc(s->w * s->h * sizeof(Pixel));

    for(int i = 0; i < s->h; i++) {
        for(int j = 0; j < s->w; j++) {
            const Uint8* const p = (Uint8*)sf->pixels + i * sf->pitch + j * sf->format->BytesPerPixel;
            Uint32 pixel;

            switch(sf->format->BytesPerPixel) {
                case 1: {
                    pixel = *p;
                } break;

                case 2: {
                    pixel = *(Uint16*)p;
                } break;

                case 3: {
                    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                        pixel = (p[0] << 16) | (p[1] << 8) | (p[2]);
                    } else {
                        pixel = (p[0]) | (p[1] << 8) | (p[2] << 16);
                    }
                } break;

                case 4: {
                    pixel = *(Uint32*)p;
                } break;

                default: {
                    pixel = 0;
                } break;
            }

            Uint8 r, g, b;
            SDL_GetRGB(pixel, sf->format, &r, &g, &b);

            s->data[i * s->w + j] = a_screen_makePixel(r, g, b);
        }
    }

    SDL_FreeSurface(sf);

    s->transparent = DEFAULT_TRANSPARENT;
    s->limit = DEFAULT_LIMIT;
    s->end = DEFAULT_END;

    return s;
}

Sheet* a_sheet__fromData(const Pixel* const data, const int w, const int h, const int size)
{
    Sheet* const s = malloc(sizeof(Sheet));

    s->w = w;
    s->h = h;
    s->data = a_mem_decodeRLE(data, size / sizeof(Pixel), sizeof(Pixel), NULL);

    s->transparent = DEFAULT_TRANSPARENT;
    s->limit = DEFAULT_LIMIT;
    s->end = DEFAULT_END;

    return s;
}

Sheet* a_sheet_fromSheet(const Sheet* const sheet, const int x, const int y, const int w, const int h)
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
    s->end = sheet->end;

    return s;
}

void a_sheet_free(Sheet* const s)
{
    free(s->data);
    free(s);
}

void a_sheet_setTransparent(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b)
{
    s->transparent = a_screen_makePixel(r, g, b);
}

void a_sheet_setLimit(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b)
{
    s->limit = a_screen_makePixel(r, g, b);
}

void a_sheet_setEnd(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b)
{
    s->end = a_screen_makePixel(r, g, b);
}

int a_sheet_w(const Sheet* const s)
{
    return s->w;
}

int a_sheet_h(const Sheet* const s)
{
    return s->h;
}

Pixel* a_sheet_data(const Sheet* const s)
{
    return s->data;
}

Pixel a_sheet_getPixel(const Sheet* const s, const int x, const int y)
{
    return *(s->data + y * s->w + x);
}
