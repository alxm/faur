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

typedef enum AFontLoad  {
    A_FONT_LOAD_ALL = 1,
    A_FONT_LOAD_ALPHANUMERIC = 2,
    A_FONT_LOAD_ALPHA = 4,
    A_FONT_LOAD_NUMERIC = 8,
    A_FONT_LOAD_CAPS = 16,
} AFontLoad;

typedef enum AFontAlign {
    A_FONT_ALIGN_LEFT = 1,
    A_FONT_ALIGN_MIDDLE = 2,
    A_FONT_ALIGN_RIGHT = 4,
    A_FONT_MONOSPACED = 8
} AFontAlign;

typedef enum AFontDefaults {
    A_FONT_WHITE,
    A_FONT_GREEN,
    A_FONT_YELLOW,
    A_FONT_RED,
    A_FONT_BLUE,
    A_FONT_MAX
} AFontDefaults;

extern int a_font_load(const ASprite* sheet, int x, int y, AFontLoad loader);
extern int a_font_copy(int font, APixel color);

extern void a_font_setFace(int f);
extern void a_font_setAlign(AFontAlign a);
extern void a_font_setCoords(int x, int y);

extern int a_font_getX(void);

extern int a_font_width(const char* text);
extern int a_font_widthf(const char* fmt, ...);

extern void a_font_text(const char* text);
extern void a_font_textf(const char* fmt, ...);

extern void a_font_fixed(int width, const char* text);

extern void a_font_int(int number);
extern void a_font_float(float number);
extern void a_font_double(double number);
extern void a_font_char(char ch);
