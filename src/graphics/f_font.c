/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "f_font.v.h"
#include <faur.v.h>

#include <faur_media/g_font_6x8.png.h>
#include <faur_media/g_font_keyed_6x8.png.h>

#define F__CHAR_START 32
#define F__CHAR_INDEX(Char) ((unsigned)Char - F__CHAR_START)
#define F__LINE_SPACING 1

static FFont* g_defaultFonts[F_FONT__ID_NUM];
static FFontState g_state;
static FList* g_stack;
static char g_buffer[512];

static void f_font__init(void)
{
    g_stack = f_list_new();

    g_defaultFonts[F_FONT__ID_BLOCK] = (FFont*)f_gfx__g_font_6x8;

    #if !F_CONFIG_TRAIT_LOW_MEM
        g_defaultFonts[F_FONT__ID_KEYED] = (FFont*)f_gfx__g_font_keyed_6x8;
    #endif

    f_font_reset();
}

static void f_font__uninit(void)
{
    for(int f = 0; f < F_FONT__ID_NUM; f++) {
        f_font_free(g_defaultFonts[f]);
    }

    f_list_freeEx(g_stack, f_pool_release);
}

const FPack f_pack__font = {
    "Font",
    f_font__init,
    f_font__uninit,
};

#if F_CONFIG_LIB_PNG
FFont* f_font_newFromPng(const char* Path, int X, int Y, int CharWidth, int CharHeight)
{
    return f_sprite_newFromPng(Path, X, Y, CharWidth, CharHeight);
}
#endif

FFont* f_font_newFromSprite(const FSprite* Sheet, int X, int Y, int CharWidth, int CharHeight)
{
    return f_sprite_newFromSprite(Sheet, X, Y, CharWidth, CharHeight);
}

void f_font_free(FFont* Font)
{
    f_sprite_free(Font);
}

void f_font_push(void)
{
    f_list_push(g_stack, f_pool__dup(F_POOL__STACK_FONT, &g_state));

    f_font_reset();
}

void f_font_pop(void)
{
    FFontState* state = f_list_pop(g_stack);

    #if F_CONFIG_DEBUG
        if(state == NULL) {
            F__FATAL("f_font_pop: Stack is empty");
        }
    #endif

    g_state = *state;
    f_pool_release(state);
}

void f_font_reset(void)
{
    f_font__fontSet(F_FONT__ID_BLOCK);
    f_font_coordsSet(0, 0);
    f_font_lineWrapSet(0);
}

void f_font_fontSet(const FFont* Font)
{
    if(Font == NULL) {
        Font = g_defaultFonts[F_FONT__ID_BLOCK];
    }

    g_state.font = Font;
    g_state.lineHeight = f_sprite_sizeGetHeight(Font) + F__LINE_SPACING;
}

void f_font__fontSet(FFontId Font)
{
    f_font_fontSet(g_defaultFonts[Font]);
}

void f_font_coordsSet(int X, int Y)
{
    g_state.x = g_state.startX = X;
    g_state.y = Y;
    g_state.currentLineWidth = 0;
}

int f_font_coordsGetX(void)
{
    return g_state.x;
}

int f_font_coordsGetY(void)
{
    return g_state.y;
}

int f_font_lineHeightGet(void)
{
    return g_state.lineHeight;
}

void f_font_lineHeightSet(int Height)
{
    g_state.lineHeight = Height;
}

void f_font_lineWrapSet(int Width)
{
    g_state.wrapWidth = Width;
    g_state.currentLineWidth = 0;
}

void f_font_lineNew(void)
{
    g_state.x = g_state.startX;
    g_state.y += g_state.lineHeight;

    g_state.currentLineWidth = 0;
}

static void drawString(const char* Text, ptrdiff_t Length)
{
    FAlign align = f__align;

    f_align_set(F_ALIGN_X_LEFT, F_ALIGN_Y_TOP);

    const FSprite* chars = g_state.font;
    int charWidth = f_sprite_sizeGetWidth(chars);

    int x = g_state.x;
    int y = g_state.y;

    if(align.x == F_ALIGN_X_CENTER) {
        x -= (int)Length * charWidth / 2;
    } else if(align.x == F_ALIGN_X_RIGHT) {
        x -= (int)Length * charWidth;
    }

    if(align.y == F_ALIGN_Y_CENTER) {
        y -= f_sprite_sizeGetHeight(chars) / 2;
    } else if(align.y == F_ALIGN_Y_BOTTOM) {
        y -= f_sprite_sizeGetHeight(chars);
    }

    for( ; Length--; Text++) {
        f_sprite_blit(chars, F__CHAR_INDEX(*Text), x, y);
        x += charWidth;
    }

    g_state.x = x;

    f__align = align;
}

static void wrapString(const char* Text)
{
    int charWidth = f_sprite_sizeGetWidth(g_state.font);

    const char* lineStart = Text;
    const char* wordStart = NULL;

    for(char ch = *Text; ch != '\0'; ch = *Text) {
        if(ch > F__CHAR_START) {
            if(wordStart == NULL) {
                wordStart = Text;
            }

            g_state.currentLineWidth += charWidth;
        } else {
            wordStart = NULL;

            if(ch == ' ') {
                if(g_state.currentLineWidth == 0) {
                    // Do not start a line with spaces
                    lineStart++;
                } else {
                    g_state.currentLineWidth += charWidth;
                }
            } else {
                // Print what we have and skip non-printable character
                drawString(lineStart, Text - lineStart);

                if(ch == '\n') {
                    f_font_lineNew();
                }

                lineStart = Text + 1;
            }
        }

        Text++;

        if(g_state.currentLineWidth > g_state.wrapWidth) {
            if(wordStart == NULL) {
                // Overflowed with whitespace, print what we have
                drawString(lineStart, Text - lineStart);
                f_font_lineNew();

                lineStart = Text;
            } else if(lineStart < wordStart) {
                // Print line up to overflowing word, and go back to the word
                drawString(lineStart, wordStart - lineStart);
                f_font_lineNew();

                Text = wordStart;
                lineStart = wordStart;
            }
        }
    }

    // Print last line
    drawString(lineStart, Text - lineStart);
}

void f_font_print(const char* Text)
{
    if(g_state.wrapWidth > 0) {
        wrapString(Text);

        return;
    }

    const char* lineStart = Text;

    for(char ch = *Text; ch != '\0'; ch = *++Text) {
        if(ch < F__CHAR_START) {
            drawString(lineStart, Text - lineStart);

            if(ch == '\n') {
                f_font_lineNew();
            }

            lineStart = Text + 1;
        }
    }

    drawString(lineStart, Text - lineStart);
}

void f_font_printf(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    f_font_printv(Format, args);

    va_end(args);
}

void f_font_printv(const char* Format, va_list Args)
{
    if(f_str_fmtv(g_buffer, sizeof(g_buffer), true, Format, Args)) {
        f_font_print(g_buffer);
    }
}

int f_font_widthGet(const char* Text)
{
    int width = 0;
    int charWidth = f_sprite_sizeGetWidth(g_state.font);

    for(char ch = *Text; ch >= F__CHAR_START; ch = *++Text) {
        width += charWidth;
    }

    return width;
}

int f_font_widthGetf(const char* Format, ...)
{
    int width;
    va_list args;

    va_start(args, Format);
    width = f_font_widthGetv(Format, args);
    va_end(args);

    return width;
}

int f_font_widthGetv(const char* Format, va_list Args)
{
    if(f_str_fmtv(g_buffer, sizeof(g_buffer), true, Format, Args)) {
        return f_font_widthGet(g_buffer);
    }

    return 0;
}
