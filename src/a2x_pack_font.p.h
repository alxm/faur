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
    A_FONT_ALIGN_MONOSPACED = 8
} AFontAlign;

typedef enum AFontDefaults {
    A_FONT_FACE_WHITE,
    A_FONT_FACE_LIGHT_GRAY,
    A_FONT_FACE_GREEN,
    A_FONT_FACE_YELLOW,
    A_FONT_FACE_RED,
    A_FONT_FACE_BLUE,
    A_FONT_FACE_DEFAULT_NUM
} AFontDefaults;

extern unsigned a_font_load(const ASprite* Sheet, int X, int Y, AFontLoad Loader);
extern unsigned a_font_copy(unsigned Font, APixel Color);

extern void a_font_push(void);
extern void a_font_pop(void);

extern void a_font_reset(void);

extern void a_font_setFace(unsigned Font);
extern void a_font_setAlign(AFontAlign Align);

extern void a_font_setCoords(int X, int Y);
extern int a_font_getX(void);
extern int a_font_getY(void);
extern void a_font_newLine(void);

extern int a_font_getLineHeight(void);
extern void a_font_setLineHeight(int Height);
extern void a_font_setWrap(int Width);

extern int a_font_width(const char* Text);
extern int a_font_widthf(const char* Format, ...);
extern int a_font_widthv(const char* Format, va_list Args);

extern void a_font_text(const char* Text);
extern void a_font_textf(const char* Format, ...);
extern void a_font_textv(const char* Format, va_list Args);
