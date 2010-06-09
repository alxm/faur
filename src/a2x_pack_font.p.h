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

#ifndef A2X_PACK_FONT_PH
#define A2X_PACK_FONT_PH

#include <stdlib.h>
#include <stdio.h>

#include "a2x_pack_blit.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_screen.p.h"
#include "a2x_pack_sprite.p.h"

typedef enum FontLoad  {
    A_LOAD_ALL = 1, A_LOAD_AN = 2, A_LOAD_A = 4, A_LOAD_N = 8, A_LOAD_CAPS = 16
} FontLoad;

typedef enum FontAlign {
    A_LEFT = 1, A_MIDDLE = 2, A_RIGHT = 4, A_SPACED = 8, A_SAFE = 16
} FontAlign;

extern int a_font_load(Sheet* const sheet, const int sx, const int sy, const int zoom, const FontLoad loader);
extern int a_font_copy(const int font, const uint8_t r, const uint8_t g, const uint8_t b);

extern int a_font_text(const FontAlign align, int x, const int y, const int index, const Blit_t draw, const char* const text);
extern int a_font_safe(const FontAlign align, int x, const int y, const int index, const Blit_t draw, const char* const text);
extern int a_font_int(const FontAlign align, const int x, const int y, const int f, const Blit_t draw, const int number);
extern int a_font_float(const FontAlign align, const int x, const int y, const int f, const Blit_t draw, const float number);
extern int a_font_char(const FontAlign align, const int x, const int y, const int f, const Blit_t draw, const char ch);

extern int a_font_fixed(const FontAlign align, const int x, const int y, const int f, const Blit_t draw, const int width, const char* const text);

extern int a_font_width(const int index, const char* const text);

#define a_font_textf(align, x, y, f, b, ...) \
({                                           \
    String256 a__s;                          \
    sprintf(a__s, __VA_ARGS__);              \
    a_font_text(align, x, y, f, b, a__s);    \
})

#define a_font_widthf(f, ...)   \
({                              \
    String256 a__s;             \
    sprintf(a__s, __VA_ARGS__); \
    a_font_width(f, a__s);      \
})

#endif // A2X_PACK_FONT_PH
