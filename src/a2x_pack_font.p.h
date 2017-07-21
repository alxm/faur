/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_system_includes.h"

typedef struct AFont AFont;

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
    A_FONT_ALIGN_MONOSPACED = 8
} AFontAlign;

#include "a2x_pack_sprite.p.h"

extern AFont* a_font_new(const ASprite* Sheet, int X, int Y, AFontLoad Loader);
extern AFont* a_font_dup(AFont* Font, APixel Color);

extern void a_font_push(void);
extern void a_font_pop(void);

extern void a_font_reset(void);

extern void a_font_setFont(AFont* Font);
extern void a_font_setAlign(AFontAlign Align);

extern void a_font_setCoords(int X, int Y);
extern int a_font_getX(void);
extern int a_font_getY(void);
extern void a_font_newLine(void);

extern int a_font_getLineHeight(void);
extern void a_font_setLineHeight(int Height);
extern void a_font_setWrap(int Width);

extern int a_font_getWidth(const char* Text);
extern int a_font_getWidthf(const char* Format, ...);
extern int a_font_getWidthv(const char* Format, va_list Args);

extern void a_font_print(const char* Text);
extern void a_font_printf(const char* Format, ...);
extern void a_font_printv(const char* Format, va_list Args);
