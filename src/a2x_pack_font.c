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

#include "a2x_pack_font.v.h"
#include "media/font.h"

#define NUM_ASCII 256

#define BLANK_SPACE  3
#define CHAR_SPACING 1
#define LINE_SPACING 1

typedef struct AFont {
    ASprite* sprites[NUM_ASCII];
    int maxWidth;
    int maxHeight;
} AFont;

static const char g_chars[] =
    "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
    "0123456789"
    "_-+=*/\\&$@!?'\"()[]{}.,~:;%^#<>|`";

#define CHARS_NUM (sizeof(g_chars) / sizeof(char) - 1)

static AList* g_fontsList;
static AFont** g_fonts;
static int g_currentFont;
static AFontAlign g_align;
static int g_x, g_initialX, g_y;
static int g_lineHeight;
static int g_wrapWidth, g_currentLineWidth;

static int charIndex(char Character)
{
    for(int i = 0; i < CHARS_NUM; i++) {
        if(g_chars[i] == Character) {
            return i;
        }
    }

    return -1;
}

void a_font__init(void)
{
    g_fontsList = a_list_new();
    g_fonts = NULL;
    g_align = A_FONT_ALIGN_LEFT;
    g_x = g_initialX = 0;
    g_y = 0;
    g_wrapWidth = 0;
    g_currentLineWidth = 0;

    ASprite* fontSprite = a_sprite_fromData(g_media_font);

    APixel colors[A_FONT_FACE_DEFAULT_NUM];
    colors[A_FONT_FACE_WHITE] = a_pixel_make(0xff, 0xff, 0xff);
    colors[A_FONT_FACE_LIGHT_GRAY] = a_pixel_make(0xaf, 0xaf, 0xaf);
    colors[A_FONT_FACE_GREEN] = a_pixel_make(0x3f, 0xbf, 0x9f);
    colors[A_FONT_FACE_YELLOW] = a_pixel_make(0x9f, 0xcf, 0x3f);
    colors[A_FONT_FACE_RED] = a_pixel_make(0xcf, 0x2f, 0x4f);
    colors[A_FONT_FACE_BLUE] = a_pixel_make(0x3f, 0x8f, 0xdf);

    a_font_load(fontSprite, 0, 0, A_FONT_LOAD_ALL);

    for(size_t f = 1; f < A_FONT_FACE_DEFAULT_NUM; f++) {
        a_font_copy(A_FONT_FACE_WHITE, colors[f]);
    }

    a_font_setFace(A_FONT_FACE_WHITE);
}

void a_font__uninit(void)
{
    A_LIST_ITERATE(g_fontsList, AFont*, f) {
        free(f);
    }

    a_list_free(g_fontsList);
    free(g_fonts);
}

size_t a_font_load(const ASprite* Sheet, int X, int Y, AFontLoad Loader)
{
    AFont* const f = a_mem_malloc(sizeof(AFont));

    for(int i = NUM_ASCII; i--; ) {
        f->sprites[i] = NULL;
    }

    f->maxWidth = 0;
    f->maxHeight = 0;

    a_list_addLast(g_fontsList, f);

    free(g_fonts);
    g_fonts = (AFont**)a_list_array(g_fontsList);

    int start = 0;
    int end = CHARS_NUM - 1;

    if(Loader & A_FONT_LOAD_ALPHANUMERIC) {
        end = charIndex('9');
    } else if(Loader & A_FONT_LOAD_ALPHA) {
        end = charIndex('z');
    } else if(Loader & A_FONT_LOAD_NUMERIC) {
        start = charIndex('0');
        end = charIndex('9');
    }

    ASpriteFrames* sf = a_spriteframes_new(Sheet, X, Y, 1);

    for(int i = start; i <= end; i++) {
        ASprite* spr = a_spriteframes_next(sf);

        f->sprites[(int)g_chars[i]] = spr;
        f->maxWidth = a_math_max(f->maxWidth, spr->w);
        f->maxHeight = a_math_max(f->maxHeight, spr->h);

        if((Loader & A_FONT_LOAD_CAPS) && isalpha(g_chars[i])) {
            f->sprites[(int)g_chars[i + 1]] = spr;
            i++;
        }
    }

    a_spriteframes_free(sf, false);

    return a_list_size(g_fontsList) - 1;
}

size_t a_font_copy(int Font, APixel Color)
{
    AFont* src = g_fonts[Font];
    AFont* f = a_mem_malloc(sizeof(AFont));

    *f = *src;

    for(int i = NUM_ASCII; i--; ) {
        if(src->sprites[i]) {
            f->sprites[i] = a_sprite_clone(src->sprites[i]);

            ASprite* sprite = f->sprites[i];
            APixel* pixels = sprite->pixels;

            for(int j = sprite->w * sprite->h; j--; pixels++) {
                if(*pixels != A_SPRITE_COLORKEY) {
                    *pixels = Color;
                }
            }
        } else {
            f->sprites[i] = NULL;
        }
    }

    a_list_addLast(g_fontsList, f);

    free(g_fonts);
    g_fonts = (AFont**)a_list_array(g_fontsList);

    return a_list_size(g_fontsList) - 1;
}

void a_font_setFace(int Font)
{
    g_currentFont = Font;
    g_lineHeight = g_fonts[g_currentFont]->maxHeight + LINE_SPACING;
}

void a_font_setAlign(AFontAlign Align)
{
    g_align = Align;
}

void a_font_setCoords(int X, int Y)
{
    g_x = g_initialX = X;
    g_y = Y;
    g_currentLineWidth = 0;
}

int a_font_getX(void)
{
    return g_x;
}

int a_font_getY(void)
{
    return g_y;
}

void a_font_newLine(void)
{
    g_x = g_initialX;
    g_y += g_lineHeight;
    g_currentLineWidth = 0;
}

int a_font_getLineHeight(void)
{
    return g_lineHeight;
}

void a_font_setLineHeight(int Height)
{
    g_lineHeight = Height;
}

void a_font_setWrap(int Width)
{
    g_wrapWidth = Width;
}

static int getWidth(const char* Text, size_t Length)
{
    int width = 0;
    AFont* font = g_fonts[g_currentFont];

    if(g_align & A_FONT_ALIGN_MONOSPACED) {
        width = (font->maxWidth + CHAR_SPACING) * Length;
    } else {
        for( ; Length--; Text++) {
            ASprite* spr = font->sprites[(int)*Text];
            width += (spr ? spr->w : BLANK_SPACE) + CHAR_SPACING;
        }
    }

    if(width > 0) {
        width -= CHAR_SPACING;
    }

    return width;
}

static void drawString(const char* Text, size_t Length)
{
    AFont* font = g_fonts[g_currentFont];

    if(g_align & A_FONT_ALIGN_MIDDLE) {
        g_x -= getWidth(Text, Length) / 2;
    } else if(g_align & A_FONT_ALIGN_RIGHT) {
        g_x -= getWidth(Text, Length);
    }

    if(g_align & A_FONT_ALIGN_MONOSPACED) {
        for( ; Length--; Text++) {
            ASprite* spr = font->sprites[(int)*Text];

            if(spr) {
                a_sprite_blit(spr, g_x + (font->maxWidth - spr->w) / 2, g_y);
            }

            g_x += font->maxWidth + CHAR_SPACING;
        }
    } else {
        for( ; Length--; Text++) {
            ASprite* spr = font->sprites[(int)*Text];

            if(spr) {
                a_sprite_blit(spr, g_x, g_y);
                g_x += spr->w + CHAR_SPACING;
            } else {
                g_x += BLANK_SPACE + CHAR_SPACING;
            }
        }
    }
}

static void wrapString(const char* Text)
{
    AFont* font = g_fonts[g_currentFont];
    const char* lineStart = Text;

    while(*Text != '\0') {
        int tally = 0;

        if(g_currentLineWidth > 0) {
            if(g_align & A_FONT_ALIGN_MONOSPACED) {
                for( ; *Text == ' '; Text++) {
                    tally += font->maxWidth + CHAR_SPACING;
                }
            } else {
                for( ; *Text == ' '; Text++) {
                    tally += BLANK_SPACE + CHAR_SPACING;
                }
            }
        } else {
            // Do not start a line with spaces
            for( ; *Text == ' '; Text++) {
                lineStart++;
            }
        }

        const char* wordStart = Text;

        if(g_currentLineWidth + tally <= g_wrapWidth) {
            if(g_align & A_FONT_ALIGN_MONOSPACED) {
                for( ; *Text != ' ' && *Text != '\0'; Text++) {
                    tally += font->maxWidth + CHAR_SPACING;
                }
            } else {
                for( ; *Text != ' ' && *Text != '\0'; Text++) {
                    ASprite* spr = font->sprites[(int)*Text];
                    tally += (spr ? spr->w : BLANK_SPACE) + CHAR_SPACING;
                }
            }
        }

        if(tally > 0) {
            tally -= CHAR_SPACING;
        }

        if(g_currentLineWidth + tally > g_wrapWidth) {
            if(g_currentLineWidth == 0) {
                // Print lone word regardless of overflow
                drawString(lineStart, Text - lineStart);
            } else {
                // Print line up to overflowing word, and go back to word
                drawString(lineStart, wordStart - lineStart);
                Text = wordStart;
            }

            lineStart = Text;
            g_currentLineWidth = 0;
            a_font_newLine();
        } else {
            if(*Text == '\0') {
                // Print last line
                drawString(lineStart, Text - lineStart);
            }

            if(tally > 0) {
                g_currentLineWidth += tally + CHAR_SPACING;
            }
        }
    }
}

void a_font_text(const char* Text)
{
    if(g_wrapWidth > 0) {
        wrapString(Text);
    } else {
        drawString(Text, strlen(Text));
    }
}

void a_font_textf(const char* Format, ...)
{
    int ret;
    va_list args;
    char buffer[256];

    va_start(args, Format);
    ret = vsnprintf(buffer, sizeof(buffer), Format, args);
    va_end(args);

    if(ret > 0) {
        a_font_text(buffer);
    }
}

int a_font_width(const char* Text)
{
    return getWidth(Text, strlen(Text));
}

int a_font_widthf(const char* Format, ...)
{
    int ret;
    va_list args;
    char buffer[256];

    va_start(args, Format);
    ret = vsnprintf(buffer, sizeof(buffer), Format, args);
    va_end(args);

    if(ret < 0) {
        return 0;
    }

    return a_font_width(buffer);
}
