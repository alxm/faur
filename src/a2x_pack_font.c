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

#include "a2x_system_includes.h"

#include "a2x_pack_font.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_spriteframes.v.h"

#define CHAR_ENTRIES_NUM 128
#define CHAR_INDEX(Char) ((unsigned)Char & (CHAR_ENTRIES_NUM - 1))

#define LINE_SPACING 1

struct AFont {
    AFontLoad loader;
    ASpriteFrames* frames;
    ASprite* sprites[CHAR_ENTRIES_NUM];
    ASprite* gapSprite;
    int gap;
    int blankWidth;
    int maxWidth;
    int maxHeight;
};

enum {
    A_FONT__NUM_ALPHA = 52,
    A_FONT__NUM_NUMERIC = 10,
    A_FONT__NUM_ALPHANUMERIC = A_FONT__NUM_ALPHA + A_FONT__NUM_NUMERIC,
    A_FONT__NUM_ALL = A_FONT__NUM_ALPHANUMERIC + 32
};

static const char g_chars[] =
    "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
    "0123456789"
    "_-+=*/\\&$@!?'\"()[]{}.,~:;%^#<>|`";

#define CHARS_NUM (sizeof(g_chars) - 1)

typedef struct {
    AFont* font;
    AFontAlign align;
    int x, startX, y;
    int lineHeight;
    int wrapWidth, currentLineWidth;
} AFontState;

AFont* g_defaultFonts[A_FONT_FACE_DEFAULT_NUM];
static AFontState g_state;
static AList* g_stateStack;

static unsigned charIndex(char Character)
{
    for(unsigned i = 0; i < CHARS_NUM; i++) {
        if(g_chars[i] == Character) {
            return i;
        }
    }

    a_out__fatal("Unsupported character '%c'", Character);
}

void a_font__init(void)
{
    g_stateStack = a_list_new();

    ASprite* fontSprite = a_sprite_newFromFile("/a2x/defaultFont");

    APixel colors[A_FONT_FACE_DEFAULT_NUM];
    colors[A_FONT_FACE_LIGHT_GRAY] = a_pixel_hex(0xafafaf);
    colors[A_FONT_FACE_GREEN] = a_pixel_hex(0x3fbf9f);
    colors[A_FONT_FACE_YELLOW] = a_pixel_hex(0x9fcf3f);
    colors[A_FONT_FACE_RED] = a_pixel_hex(0xcf2f4f);
    colors[A_FONT_FACE_BLUE] = a_pixel_hex(0x3f8fdf);

    g_defaultFonts[A_FONT_FACE_DEFAULT] = a_font_newFromSprite(fontSprite,
                                                               0,
                                                               9,
                                                               A_FONT_LOAD_ALL);

    g_defaultFonts[A_FONT_FACE_WHITE] = a_font_newFromSprite(fontSprite,
                                                             0,
                                                             0,
                                                             A_FONT_LOAD_ALL);

    a_sprite_free(fontSprite);

    for(AFontDefaults f = A_FONT_FACE_WHITE + 1;
        f < A_FONT_FACE_DEFAULT_NUM;
        f++) {

        g_defaultFonts[f] = a_font_dup(g_defaultFonts[A_FONT_FACE_WHITE],
                                       colors[f]);
    }

    a_font_reset();
}

void a_font__uninit(void)
{
    for(AFontDefaults f = 0; f < A_FONT_FACE_DEFAULT_NUM; f++) {
        a_font_free(g_defaultFonts[f]);
    }

    a_list_freeEx(g_stateStack, free);
}

static AFont* a_font__new(AFontLoad Loader, ASpriteFrames* Frames)
{
    AFont* f = a_mem_malloc(sizeof(AFont));

    f->loader = Loader;
    f->frames = Frames;

    unsigned numFrames = a_spriteframes_getNum(Frames);
    bool hasGap = numFrames > A_FONT__NUM_ALL + 1;

    unsigned start = 0;
    unsigned end = CHARS_NUM - 1;

    if(Loader & A_FONT_LOAD_ALPHANUMERIC) {
        end = charIndex('9');
        hasGap = numFrames > A_FONT__NUM_ALPHANUMERIC + 1;
    } else if(Loader & A_FONT_LOAD_ALPHA) {
        end = charIndex('z');
        hasGap = numFrames > A_FONT__NUM_ALPHA + 1;
    } else if(Loader & A_FONT_LOAD_NUMERIC) {
        start = charIndex('0');
        end = charIndex('9');
        hasGap = numFrames > A_FONT__NUM_NUMERIC + 1;
    }

    ASprite* gapSprite = hasGap ? a_spriteframes_getNext(Frames) : NULL;
    ASprite* blankSprite = a_spriteframes_getNext(Frames);

    for(int i = CHAR_ENTRIES_NUM; i--; ) {
        f->sprites[i] = blankSprite;
    }

    f->gapSprite = gapSprite;
    f->gap = gapSprite ? gapSprite->w : 0;
    f->blankWidth = blankSprite->w;
    f->maxWidth = blankSprite->w;
    f->maxHeight = blankSprite->h;

    for(unsigned i = start; i <= end; i++) {
        ASprite* spr = a_spriteframes_getNext(Frames);

        f->sprites[CHAR_INDEX(g_chars[i])] = spr;
        f->maxWidth = a_math_max(f->maxWidth, spr->w);
        f->maxHeight = a_math_max(f->maxHeight, spr->h);

        if((Loader & A_FONT_LOAD_CAPS) && isalpha(g_chars[i])) {
            f->sprites[CHAR_INDEX(g_chars[i + 1])] = spr;
            i++;
        }
    }

    return f;
}

AFont* a_font_newFromFile(const char* Path, AFontLoad Loader)
{
    ASprite* s = a_sprite_newFromFile(Path);
    AFont* f = a_font_newFromSprite(s, 0, 0, Loader);

    a_sprite_free(s);

    return f;
}

AFont* a_font_newFromSprite(const ASprite* Sheet, int X, int Y, AFontLoad Loader)
{
    ASpriteFrames* frames = a_spriteframes_newFromSprite(Sheet, X, Y, 0);

    return a_font__new(Loader, frames);
}

AFont* a_font_dup(AFont* Font, APixel Color)
{
    a_pixel_push();
    a_pixel_setPixel(Color);
    a_pixel_setFillBlit(true);

    ASpriteFrames* frames = a_spriteframes_dup(Font->frames, true);

    for(unsigned i = a_spriteframes_getNum(frames); i--; ) {
        ASprite* sprite = a_spriteframes_getNext(frames);

        a_screen_targetPushSprite(sprite);
        a_sprite_blit(sprite, 0, 0);
        a_screen_targetPop();
    }

    a_pixel_pop();

    return a_font__new(Font->loader, frames);
}

void a_font_free(AFont* Font)
{
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
        a_out__fatal("Cannot pop AFontState: stack is empty");
    }

    g_state = *state;
    free(state);
}

void a_font_reset(void)
{
    a_font__setFont(A_FONT_FACE_DEFAULT);
    a_font_setAlign(A_FONT_ALIGN_LEFT);
    a_font_setCoords(0, 0);
    a_font_setWrap(0);
}

void a_font_setFont(AFont* Font)
{
    if(Font == NULL) {
        Font = g_defaultFonts[A_FONT_FACE_DEFAULT];
    }

    g_state.font = Font;
    g_state.lineHeight = Font->maxHeight + LINE_SPACING;
}

void a_font__setFont(AFontDefaults Font)
{
    a_font_setFont(g_defaultFonts[Font]);
}

void a_font_setAlign(AFontAlign Align)
{
    g_state.align = Align;
}

void a_font_setCoords(int X, int Y)
{
    g_state.x = g_state.startX = X;
    g_state.y = Y;
    g_state.currentLineWidth = 0;
}

int a_font_getX(void)
{
    return g_state.x;
}

int a_font_getY(void)
{
    return g_state.y;
}

void a_font_newLine(void)
{
    g_state.x = g_state.startX;
    g_state.y += g_state.lineHeight;
    g_state.currentLineWidth = 0;
}

int a_font_getLineHeight(void)
{
    return g_state.lineHeight;
}

void a_font_setLineHeight(int Height)
{
    g_state.lineHeight = Height;
}

void a_font_setWrap(int Width)
{
    g_state.wrapWidth = Width;
    g_state.currentLineWidth = 0;
}

static int getWidth(const char* Text, ptrdiff_t Length)
{
    int width = 0;
    AFont* font = g_state.font;

    if(g_state.align & A_FONT_ALIGN_MONOSPACED) {
        width = (font->maxWidth + font->gap) * (int)Length;
    } else {
        for( ; Length--; Text++) {
            width += font->sprites[CHAR_INDEX(*Text)]->w + font->gap;
        }
    }

    return width;
}

static void drawString(const char* Text, ptrdiff_t Length)
{
    AFont* font = g_state.font;

    if(g_state.align & A_FONT_ALIGN_MIDDLE) {
        g_state.x -= getWidth(Text, Length) / 2;
    } else if(g_state.align & A_FONT_ALIGN_RIGHT) {
        g_state.x -= getWidth(Text, Length);
    }

    if(g_state.align & A_FONT_ALIGN_MONOSPACED) {
        for( ; Length--; Text++) {
            ASprite* spr = font->sprites[CHAR_INDEX(*Text)];
            int xOffset = (font->maxWidth - spr->w) / 2;

            a_sprite_blit(spr, g_state.x + xOffset, g_state.y);
            g_state.x += font->maxWidth;

            if(font->gap > 0) {
                a_sprite_blit(font->gapSprite, g_state.x, g_state.y);
                g_state.x += font->gap;
            }
        }
    } else {
        for( ; Length--; Text++) {
            ASprite* spr = font->sprites[CHAR_INDEX(*Text)];

            a_sprite_blit(spr, g_state.x, g_state.y);
            g_state.x += spr->w;

            if(font->gap > 0) {
                a_sprite_blit(font->gapSprite, g_state.x, g_state.y);
                g_state.x += font->gap;
            }
        }
    }
}

static void wrapString(const char* Text)
{
    AFont* font = g_state.font;
    const char* lineStart = Text;

    while(*Text != '\0') {
        int tally = 0;

        if(g_state.currentLineWidth > 0) {
            if(g_state.align & A_FONT_ALIGN_MONOSPACED) {
                for( ; *Text == ' '; Text++) {
                    tally += font->maxWidth + font->gap;
                }
            } else {
                for( ; *Text == ' '; Text++) {
                    tally += font->blankWidth + font->gap;
                }
            }
        } else {
            // Do not start a line with spaces
            for( ; *Text == ' '; Text++) {
                lineStart++;
            }
        }

        const char* wordStart = Text;

        if(g_state.currentLineWidth + tally <= g_state.wrapWidth) {
            if(g_state.align & A_FONT_ALIGN_MONOSPACED) {
                for( ; *Text != ' ' && *Text != '\0'; Text++) {
                    tally += font->maxWidth + font->gap;
                }
            } else {
                for( ; *Text != ' ' && *Text != '\0'; Text++) {
                    tally += font->sprites[CHAR_INDEX(*Text)]->w + font->gap;
                }
            }
        }

        if(tally > 0) {
            tally -= font->gap;
        }

        if(g_state.currentLineWidth + tally > g_state.wrapWidth) {
            if(g_state.currentLineWidth == 0) {
                // Print lone word regardless of overflow
                drawString(lineStart, Text - lineStart);
            } else {
                // Print line up to overflowing word, and go back to word
                drawString(lineStart, wordStart - lineStart);
                Text = wordStart;
            }

            lineStart = Text;
            g_state.currentLineWidth = 0;
            a_font_newLine();
        } else {
            if(*Text == '\0') {
                // Print last line
                drawString(lineStart, Text - lineStart);
            }

            if(tally > 0) {
                g_state.currentLineWidth += tally + font->gap;
            }
        }
    }
}

void a_font_print(const char* Text)
{
    if(g_state.wrapWidth > 0) {
        wrapString(Text);
    } else {
        drawString(Text, (ptrdiff_t)strlen(Text));
    }
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

int a_font_getWidth(const char* Text)
{
    return getWidth(Text, (ptrdiff_t)strlen(Text));
}

int a_font_getWidthf(const char* Format, ...)
{
    int width;
    va_list args;

    va_start(args, Format);
    width = a_font_getWidthv(Format, args);
    va_end(args);

    return width;
}

int a_font_getWidthv(const char* Format, va_list Args)
{
    char buffer[256];

    if(vsnprintf(buffer, sizeof(buffer), Format, Args) > 0) {
        return a_font_getWidth(buffer);
    }

    return 0;
}
