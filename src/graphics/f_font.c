/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "generated/media/font_6x8.png.h"
#include "generated/media/font_keyed_6x8.png.h"

#define F__CHAR_START 32
#define F__CHAR_INDEX(Char) ((unsigned)Char - F__CHAR_START)
#define F__LINE_SPACING 1

typedef struct {
    const FFont* font;
    FFontAlign align;
    int x, startX, y;
    int lineHeight;
    int wrapWidth, currentLineWidth;
} FFontState;

static FFont* g_defaultFonts[F_FONT__ID_NUM];
static FFontState g_state;
static FList* g_stateStack;
static char g_buffer[512];

static void f_font__init(void)
{
    g_stateStack = f_list_new();

    g_defaultFonts[F_FONT__ID_DEFAULT] = (FFont*)f_gfx__font_6x8;

    #if !F_CONFIG_SYSTEM_GAMEBUINO
        g_defaultFonts[F_FONT__ID_WHITE] = (FFont*)f_gfx__font_keyed_6x8;

        FColorPixel colors[F_FONT__ID_NUM] = {
            [F_FONT__ID_LIGHT_GRAY] = f_color_pixelFromHex(0xaf9898),
            [F_FONT__ID_GREEN] = f_color_pixelFromHex(0x4fbf9f),
            [F_FONT__ID_YELLOW] = f_color_pixelFromHex(0xa8cf3f),
            [F_FONT__ID_RED] = f_color_pixelFromHex(0xcf2f4f),
            [F_FONT__ID_BLUE] = f_color_pixelFromHex(0x4f8fdf),
        };

        for(int f = F_FONT__ID_WHITE + 1; f < F_FONT__ID_NUM; f++) {
            g_defaultFonts[f] = f_font_dup(g_defaultFonts[F_FONT__ID_WHITE],
                                           colors[f]);
        }
    #endif

    f_font_reset();
}

static void f_font__uninit(void)
{
    for(int f = 0; f < F_FONT__ID_NUM; f++) {
        f_font_free(g_defaultFonts[f]);
    }

    f_list_freeEx(g_stateStack, f_mem_free);
}

const FPack f_pack__font = {
    "Font",
    {
        [0] = f_font__init,
    },
    {
        [0] = f_font__uninit,
    },
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

FFont* f_font_dup(const FFont* Font, FColorPixel Color)
{
    FFont* f = f_sprite_dup(Font);

    f_color_push();
    f_color_colorSetPixel(Color);
    f_color_fillBlitSet(true);

    for(unsigned i = f_sprite_framesNumGet(f); i--; ) {
        f_screen_push(f, i);
        f_sprite_blit(f, i, 0, 0);
        f_screen_pop();
    }

    f_color_pop();

    return f;
}

void f_font_free(FFont* Font)
{
    f_sprite_free(Font);
}

void f_font_push(void)
{
    f_list_push(g_stateStack, f_mem_dup(&g_state, sizeof(FFontState)));
}

void f_font_pop(void)
{
    FFontState* state = f_list_pop(g_stateStack);

    #if F_CONFIG_BUILD_DEBUG
        if(state == NULL) {
            F__FATAL("f_font_pop: Stack is empty");
        }
    #endif

    g_state = *state;
    f_mem_free(state);
}

void f_font_reset(void)
{
    f_font__fontSet(F_FONT__ID_DEFAULT);
    f_font_alignSet(F_FONT_ALIGN_LEFT);
    f_font_coordsSet(0, 0);
    f_font_lineWrapSet(0);
}

void f_font_fontSet(const FFont* Font)
{
    if(Font == NULL) {
        Font = g_defaultFonts[F_FONT__ID_DEFAULT];
    }

    g_state.font = Font;
    g_state.lineHeight = f_sprite_sizeGetHeight(Font) + F__LINE_SPACING;
}

void f_font__fontSet(FFontId Font)
{
    f_font_fontSet(g_defaultFonts[Font]);
}

void f_font_alignSet(FFontAlign Align)
{
    g_state.align = Align;
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
    const FSprite* chars = g_state.font;
    int charWidth = f_sprite_sizeGetWidth(chars);

    if(g_state.align == F_FONT_ALIGN_MIDDLE) {
        g_state.x -= (int)Length * charWidth / 2;
    } else if(g_state.align == F_FONT_ALIGN_RIGHT) {
        g_state.x -= (int)Length * charWidth;
    }

    for( ; Length--; Text++) {
        f_sprite_blit(chars, F__CHAR_INDEX(*Text), g_state.x, g_state.y);
        g_state.x += charWidth;
    }
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
    f_sprite_alignReset();

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
