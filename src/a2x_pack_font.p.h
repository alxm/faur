/*
    Copyright 2010, 2016, 2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_system_includes.h"

typedef struct AFont AFont;

typedef enum {
    A_FONT_ALIGN_INVALID = -1,
    A_FONT_ALIGN_LEFT,
    A_FONT_ALIGN_MIDDLE,
    A_FONT_ALIGN_RIGHT,
} AFontAlign;

#include "a2x_pack_sprite.p.h"

extern AFont* a_font_newFromPng(const char* Path);
extern AFont* a_font_newFromSprite(const ASprite* Sheet, int X, int Y);
extern AFont* a_font_dup(const AFont* Font, APixel Color);
extern void a_font_free(AFont* Font);

extern void a_font_push(void);
extern void a_font_pop(void);

extern void a_font_reset(void);

extern void a_font_fontSet(const AFont* Font);
extern void a_font_alignSet(AFontAlign Align);

extern void a_font_coordsSet(int X, int Y);
extern int a_font_coordsGetX(void);
extern int a_font_coordsGetY(void);
extern void a_font_newLine(void);

extern int a_font_lineHeightGet(void);
extern void a_font_lineHeightSet(int Height);
extern void a_font_lineWrapSet(int Width);

extern int a_font_widthGet(const char* Text);
extern int a_font_widthGetf(const char* Format, ...);
extern int a_font_widthGetv(const char* Format, va_list Args);

extern void a_font_print(const char* Text);
extern void a_font_printf(const char* Format, ...);
extern void a_font_printv(const char* Format, va_list Args);
