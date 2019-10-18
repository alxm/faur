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

#include "a_font.v.h"
#include <faur.v.h>

#define A__CHAR_START 32
#define A__CHAR_INDEX(Char) ((unsigned)Char - A__CHAR_START)
#define A__LINE_SPACING 1

typedef struct {
    const AFont* font;
    AFontAlign align;
    int x, startX, y;
    int lineHeight;
    int wrapWidth, currentLineWidth;
} AFontState;

static AFont* g_defaultFonts[A_FONT__ID_NUM];
static AFontState g_state;
static AList* g_stateStack;
static char g_buffer[512];

static void a_font__init(void)
{
    g_stateStack = a_list_new();

    #if A_CONFIG_LIB_PNG
        APixel colors[A_FONT__ID_NUM] = {
            [A_FONT__ID_LIGHT_GRAY] = a_pixel_fromHex(0xaf9898),
            [A_FONT__ID_GREEN] = a_pixel_fromHex(0x4fbf9f),
            [A_FONT__ID_YELLOW] = a_pixel_fromHex(0xa8cf3f),
            [A_FONT__ID_RED] = a_pixel_fromHex(0xcf2f4f),
            [A_FONT__ID_BLUE] = a_pixel_fromHex(0x4f8fdf),
        };

        g_defaultFonts[A_FONT__ID_DEFAULT] = a_font_newFromPng(
                                                "/faur/font", 0, 0, 6, 8);
        g_defaultFonts[A_FONT__ID_WHITE] = a_font_newFromPng(
                                            "/faur/fontKeyed", 0, 0, 6, 8);

        for(int f = A_FONT__ID_WHITE + 1; f < A_FONT__ID_NUM; f++) {
            g_defaultFonts[f] = a_font_dup(g_defaultFonts[A_FONT__ID_WHITE],
                                           colors[f]);
        }

        a_font_reset();
    #endif
}

static void a_font__uninit(void)
{
    for(int f = 0; f < A_FONT__ID_NUM; f++) {
        a_font_free(g_defaultFonts[f]);
    }

    a_list_freeEx(g_stateStack, a_mem_free);
}

const APack a_pack__font = {
    "Font",
    {
        [0] = a_font__init,
    },
    {
        [0] = a_font__uninit,
    },
};

#if A_CONFIG_LIB_PNG
AFont* a_font_newFromPng(const char* Path, int X, int Y, int CharWidth, int CharHeight)
{
    return a_sprite_newFromPng(Path, X, Y, CharWidth, CharHeight);
}
#endif

AFont* a_font_newFromSprite(const ASprite* Sheet, int X, int Y, int CharWidth, int CharHeight)
{
    return a_sprite_newFromSprite(Sheet, X, Y, CharWidth, CharHeight);
}

AFont* a_font_dup(const AFont* Font, APixel Color)
{
    AFont* f = a_sprite_dup(Font);

    a_color_push();
    a_color_baseSetPixel(Color);
    a_color_fillBlitSet(true);

    for(unsigned i = a_sprite_framesNumGet(f); i--; ) {
        a_screen_push(f, i);
        a_sprite_blit(f, i, 0, 0);
        a_screen_pop();
    }

    a_color_pop();

    return f;
}

void a_font_free(AFont* Font)
{
    a_sprite_free(Font);
}

void a_font_push(void)
{
    a_list_push(g_stateStack, a_mem_dup(&g_state, sizeof(AFontState)));
}

void a_font_pop(void)
{
    AFontState* state = a_list_pop(g_stateStack);

    #if A_CONFIG_BUILD_DEBUG
        if(state == NULL) {
            A__FATAL("a_font_pop: Stack is empty");
        }
    #endif

    g_state = *state;
    a_mem_free(state);
}

void a_font_reset(void)
{
    a_font__fontSet(A_FONT__ID_DEFAULT);
    a_font_alignSet(A_FONT_ALIGN_LEFT);
    a_font_coordsSet(0, 0);
    a_font_lineWrapSet(0);
}

void a_font_fontSet(const AFont* Font)
{
    if(Font == NULL) {
        Font = g_defaultFonts[A_FONT__ID_DEFAULT];
    }

    g_state.font = Font;
    g_state.lineHeight = a_sprite_sizeGetHeight(Font) + A__LINE_SPACING;
}

void a_font__fontSet(AFontId Font)
{
    a_font_fontSet(g_defaultFonts[Font]);
}

void a_font_alignSet(AFontAlign Align)
{
    g_state.align = Align;
}

void a_font_coordsSet(int X, int Y)
{
    g_state.x = g_state.startX = X;
    g_state.y = Y;
    g_state.currentLineWidth = 0;
}

int a_font_coordsGetX(void)
{
    return g_state.x;
}

int a_font_coordsGetY(void)
{
    return g_state.y;
}

int a_font_lineHeightGet(void)
{
    return g_state.lineHeight;
}

void a_font_lineHeightSet(int Height)
{
    g_state.lineHeight = Height;
}

void a_font_lineWrapSet(int Width)
{
    g_state.wrapWidth = Width;
    g_state.currentLineWidth = 0;
}

void a_font_lineNew(void)
{
    g_state.x = g_state.startX;
    g_state.y += g_state.lineHeight;

    g_state.currentLineWidth = 0;
}

static void drawString(const char* Text, ptrdiff_t Length)
{
    const ASprite* chars = g_state.font;
    int charWidth = a_sprite_sizeGetWidth(chars);

    if(g_state.align == A_FONT_ALIGN_MIDDLE) {
        g_state.x -= (int)Length * charWidth / 2;
    } else if(g_state.align == A_FONT_ALIGN_RIGHT) {
        g_state.x -= (int)Length * charWidth;
    }

    for( ; Length--; Text++) {
        a_sprite_blit(chars, A__CHAR_INDEX(*Text), g_state.x, g_state.y);
        g_state.x += charWidth;
    }
}

static void wrapString(const char* Text)
{
    int charWidth = a_sprite_sizeGetWidth(g_state.font);

    const char* lineStart = Text;
    const char* wordStart = NULL;

    for(char ch = *Text; ch != '\0'; ch = *Text) {
        if(ch > A__CHAR_START) {
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
                    a_font_lineNew();
                }

                lineStart = Text + 1;
            }
        }

        Text++;

        if(g_state.currentLineWidth > g_state.wrapWidth) {
            if(wordStart == NULL) {
                // Overflowed with whitespace, print what we have
                drawString(lineStart, Text - lineStart);
                a_font_lineNew();

                lineStart = Text;
            } else if(lineStart < wordStart) {
                // Print line up to overflowing word, and go back to the word
                drawString(lineStart, wordStart - lineStart);
                a_font_lineNew();

                Text = wordStart;
                lineStart = wordStart;
            }
        }
    }

    // Print last line
    drawString(lineStart, Text - lineStart);
}

void a_font_print(const char* Text)
{
    if(g_state.wrapWidth > 0) {
        wrapString(Text);

        return;
    }

    const char* lineStart = Text;

    for(char ch = *Text; ch != '\0'; ch = *++Text) {
        if(ch < A__CHAR_START) {
            drawString(lineStart, Text - lineStart);

            if(ch == '\n') {
                a_font_lineNew();
            }

            lineStart = Text + 1;
        }
    }

    drawString(lineStart, Text - lineStart);
}

void a_font_printf(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    a_font_printv(Format, args);

    va_end(args);
}

void a_font_printv(const char* Format, va_list Args)
{
    if(a_str_fmtv(g_buffer, sizeof(g_buffer), true, Format, Args)) {
        a_font_print(g_buffer);
    }
}

int a_font_widthGet(const char* Text)
{
    int width = 0;
    int charWidth = a_sprite_sizeGetWidth(g_state.font);

    for(char ch = *Text; ch >= A__CHAR_START; ch = *++Text) {
        width += charWidth;
    }

    return width;
}

int a_font_widthGetf(const char* Format, ...)
{
    int width;
    va_list args;

    va_start(args, Format);
    width = a_font_widthGetv(Format, args);
    va_end(args);

    return width;
}

int a_font_widthGetv(const char* Format, va_list Args)
{
    if(a_str_fmtv(g_buffer, sizeof(g_buffer), true, Format, Args)) {
        return a_font_widthGet(g_buffer);
    }

    return 0;
}
