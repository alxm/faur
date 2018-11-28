/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_font.v.h"

#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_spriteframes.v.h"

#define A__CHAR_START 32
#define A__CHAR_INDEX(Char) ((unsigned)Char - A__CHAR_START)
#define A__LINE_SPACING 1

struct AFont {
    ASpriteFrames* frames;
};

typedef struct {
    AFont* font;
    AFontAlign align;
    int x, startX, y;
    int lineHeight;
    int wrapWidth, currentLineWidth;
} AFontState;

AFont* g_defaultFonts[A_FONT__ID_NUM];
static AFontState g_state;
static AList* g_stateStack;

void a_font__init(void)
{
    g_stateStack = a_list_new();

    APixel colors[A_FONT__ID_NUM] = {
        [A_FONT__ID_LIGHT_GRAY] = a_pixel_fromHex(0xaf9898),
        [A_FONT__ID_GREEN] = a_pixel_fromHex(0x4fbf9f),
        [A_FONT__ID_YELLOW] = a_pixel_fromHex(0xa8cf3f),
        [A_FONT__ID_RED] = a_pixel_fromHex(0xcf2f4f),
        [A_FONT__ID_BLUE] = a_pixel_fromHex(0x4f8fdf),
    };

    g_defaultFonts[A_FONT__ID_DEFAULT] = a_font_newFromFile("/a2x/font");
    g_defaultFonts[A_FONT__ID_WHITE] = a_font_newFromFile("/a2x/fontKeyed");

    for(AFontId f = A_FONT__ID_WHITE + 1; f < A_FONT__ID_NUM; f++) {
        g_defaultFonts[f] = a_font_dup(
                                g_defaultFonts[A_FONT__ID_WHITE], colors[f]);
    }

    a_font_reset();
}

void a_font__uninit(void)
{
    for(AFontId f = 0; f < A_FONT__ID_NUM; f++) {
        a_font_free(g_defaultFonts[f]);
    }

    a_list_freeEx(g_stateStack, free);
}

static AFont* a_font__new(ASpriteFrames* Frames)
{
    AFont* f = a_mem_malloc(sizeof(AFont));

    f->frames = Frames;

    return f;
}

AFont* a_font_newFromFile(const char* Path)
{
    ASprite* s = a_sprite_newFromFile(Path);
    AFont* f = a_font_newFromSprite(s, 0, 0);

    a_sprite_free(s);

    return f;
}

AFont* a_font_newFromSprite(const ASprite* Sheet, int X, int Y)
{
    ASpriteFrames* frames = a_spriteframes_newFromSpriteGrid(
                                Sheet,
                                X,
                                Y,
                                Sheet->w / 16,
                                Sheet->h / 6,
                                0);

    return a_font__new(frames);
}

AFont* a_font_dup(const AFont* Font, APixel Color)
{
    a_pixel_push();
    a_pixel_colorSetPixel(Color);
    a_pixel_fillBlitSet(true);

    ASpriteFrames* frames = a_spriteframes_dup(Font->frames, true);

    for(unsigned i = a_spriteframes_numGet(frames); i--; ) {
        ASprite* sprite = a_spriteframes_getNext(frames);

        a_screen_targetPushSprite(sprite);
        a_sprite_blit(sprite, 0, 0);
        a_screen_targetPop();
    }

    a_pixel_pop();

    return a_font__new(frames);
}

void a_font_free(AFont* Font)
{
    if(Font == NULL) {
        return;
    }

    a_spriteframes_free(Font->frames, true);

    free(Font);
}

void a_font_push(void)
{
    a_list_push(g_stateStack, a_mem_dup(&g_state, sizeof(AFontState)));
}

void a_font_pop(void)
{
    AFontState* state = a_list_pop(g_stateStack);

    if(state == NULL) {
        a_out__fatal("a_font_pop: Stack is empty");
    }

    g_state = *state;
    free(state);
}

void a_font_reset(void)
{
    a_font__fontSet(A_FONT__ID_DEFAULT);
    a_font_alignSet(A_FONT_ALIGN_LEFT);
    a_font_coordsSet(0, 0);
    a_font_lineWrapSet(0);
}

void a_font_fontSet(AFont* Font)
{
    if(Font == NULL) {
        Font = g_defaultFonts[A_FONT__ID_DEFAULT];
    }

    g_state.font = Font;
    g_state.lineHeight =
        a_spriteframes_getByIndex(Font->frames, A__CHAR_INDEX(' '))->h
            + A__LINE_SPACING;
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

void a_font_newLine(void)
{
    g_state.x = g_state.startX;
    g_state.y += g_state.lineHeight;

    g_state.currentLineWidth = 0;
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

static int getWidth(const char* Text, ptrdiff_t Length)
{
    int width = 0;
    const ASpriteFrames* f = g_state.font->frames;

    for( ; Length--; Text++) {
        width += a_spriteframes_getByIndex(f, A__CHAR_INDEX(*Text))->w;
    }

    return width;
}

static void drawString(const char* Text, ptrdiff_t Length)
{
    const ASpriteFrames* f = g_state.font->frames;

    if(g_state.align == A_FONT_ALIGN_MIDDLE) {
        g_state.x -= getWidth(Text, Length) / 2;
    } else if(g_state.align == A_FONT_ALIGN_RIGHT) {
        g_state.x -= getWidth(Text, Length);
    }

    for( ; Length--; Text++) {
        const ASprite* s = a_spriteframes_getByIndex(f, A__CHAR_INDEX(*Text));

        a_sprite_blit(s, g_state.x, g_state.y);
        g_state.x += s->w;
    }
}

static void wrapString(const char* Text)
{
    const ASpriteFrames* f = g_state.font->frames;

    const char* lineStart = Text;
    const char* wordStart = NULL;

    for(char ch = *Text; ch != '\0'; ch = *Text) {
        if(ch > A__CHAR_START) {
            if(wordStart == NULL) {
                wordStart = Text;
            }

            g_state.currentLineWidth +=
                a_spriteframes_getByIndex(f, A__CHAR_INDEX(ch))->w;
        } else {
            wordStart = NULL;

            if(ch == ' ') {
                if(g_state.currentLineWidth == 0) {
                    // Do not start a line with spaces
                    lineStart++;
                } else {
                    g_state.currentLineWidth +=
                        a_spriteframes_getByIndex(f, A__CHAR_INDEX(' '))->w;
                }
            } else {
                // Print what we have and skip non-printable character
                drawString(lineStart, Text - lineStart);

                if(ch == '\n') {
                    a_font_newLine();
                }

                lineStart = Text + 1;
            }
        }

        Text++;

        if(g_state.currentLineWidth > g_state.wrapWidth) {
            if(wordStart == NULL) {
                // Overflowed with whitespace, print what we have
                drawString(lineStart, Text - lineStart);
                a_font_newLine();

                lineStart = Text;
            } else if(lineStart < wordStart) {
                // Print line up to overflowing word, and go back to the word
                drawString(lineStart, wordStart - lineStart);
                a_font_newLine();

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
                a_font_newLine();
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
    char buffer[256];

    if(vsnprintf(buffer, sizeof(buffer), Format, Args) > 0) {
        a_font_print(buffer);
    }
}

int a_font_widthGet(const char* Text)
{
    int width = 0;
    const ASpriteFrames* f = g_state.font->frames;

    for(char ch = *Text; ch >= A__CHAR_START; ch = *++Text) {
        width += a_spriteframes_getByIndex(f, A__CHAR_INDEX(ch))->w;
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
    char buffer[256];

    if(vsnprintf(buffer, sizeof(buffer), Format, Args) > 0) {
        return a_font_widthGet(buffer);
    }

    return 0;
}
