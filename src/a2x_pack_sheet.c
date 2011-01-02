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

#include "a2x_pack_sheet.v.h"

Sheet* a_sheet_fromFile(const char* const path)
{
    Sheet* const s = malloc(sizeof(Sheet));

    a2x_png_readFile(path, &s->data, &s->w, &s->h);

    s->transparent = DEFAULT_TRANSPARENT;
    s->limit = DEFAULT_LIMIT;
    s->end = DEFAULT_END;

    return s;
}

Sheet* a_sheet_fromData(const uint8_t* const data)
{
    Sheet* const s = malloc(sizeof(Sheet));

    a2x_png_readMemory(data, &s->data, &s->w, &s->h);

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
    s->transparent = a_pixel_make(r, g, b);
}

void a_sheet_setLimit(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b)
{
    s->limit = a_pixel_make(r, g, b);
}

void a_sheet_setEnd(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b)
{
    s->end = a_pixel_make(r, g, b);
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
