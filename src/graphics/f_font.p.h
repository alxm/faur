/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_GRAPHICS_FONT_P_H
#define F_INC_GRAPHICS_FONT_P_H

#include "general/f_system_includes.h"

typedef struct FSprite FFont;

typedef enum {
    F_FONT_ALIGN_INVALID = -1,
    F_FONT_ALIGN_LEFT,
    F_FONT_ALIGN_MIDDLE,
    F_FONT_ALIGN_RIGHT,
} FFontAlign;

#include "graphics/f_sprite.p.h"

extern FFont* f_font_newFromPng(const char* Path, int X, int Y, int CharWidth, int CharHeight);
extern FFont* f_font_newFromSprite(const FSprite* Sheet, int X, int Y, int CharWidth, int CharHeight);
extern FFont* f_font_dup(const FFont* Font, FColorPixel Color);
extern void f_font_free(FFont* Font);

extern void f_font_push(void);
extern void f_font_pop(void);

extern void f_font_reset(void);

extern void f_font_fontSet(const FFont* Font);
extern void f_font_alignSet(FFontAlign Align);

extern void f_font_coordsSet(int X, int Y);
extern int f_font_coordsGetX(void);
extern int f_font_coordsGetY(void);

extern int f_font_lineHeightGet(void);
extern void f_font_lineHeightSet(int Height);
extern void f_font_lineWrapSet(int Width);
extern void f_font_lineNew(void);

extern int f_font_widthGet(const char* Text);
extern int f_font_widthGetf(const char* Format, ...);
extern int f_font_widthGetv(const char* Format, va_list Args);

extern void f_font_print(const char* Text);
extern void f_font_printf(const char* Format, ...);
extern void f_font_printv(const char* Format, va_list Args);

#endif // F_INC_GRAPHICS_FONT_P_H
