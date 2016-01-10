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

#pragma once

#include "a2x_app_includes.h"

#include "a2x_pack_sprite.p.h"

typedef enum FontLoad  {
    A_LOAD_ALL = 1, A_LOAD_AN = 2, A_LOAD_A = 4, A_LOAD_N = 8, A_LOAD_CAPS = 16
} FontLoad;

typedef enum FontAlign {
    A_LEFT = 1, A_MIDDLE = 2, A_RIGHT = 4, A_SPACED = 8
} FontAlign;

extern int a_font_load(const Sprite* sheet, int x, int y, int zoom, FontLoad loader);
extern int a_font_copy(int font, uint8_t r, uint8_t g, uint8_t b);

extern void a_font_setFace(int f);
extern void a_font_setAlign(FontAlign a);
extern void a_font_setCoords(int x, int y);
extern int a_font_getX(void);

extern void a_font_text(const char* text);
extern void a_font_int(int number);
extern void a_font_float(float number);
extern void a_font_double(double number);
extern void a_font_char(char ch);

extern void a_font_fixed(int width, const char* text);

extern int a_font_width(const char* text);

#define a_font_textf(...)       \
({                              \
    char a__s[256];             \
    sprintf(a__s, __VA_ARGS__); \
    a_font_text(a__s);          \
})

#define a_font_widthf(f, ...)   \
({                              \
    char a__s[256];             \
    sprintf(a__s, __VA_ARGS__); \
    a_font_width(a__s);         \
})
