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
static int g_x, g_initialX;
static int g_y;
static int g_lineWidth;

static int charIndex(char Character)
{
    for(int i = 0; i < CHARS_NUM; i++) {
        if(g_chars[i] == Character) return i;
    }

    return -1;
}

static bool shortenText(const char* Text, char** NewText)
{
    int tally = 0;
    int numChars = 0;
    const AFont* const f = g_fonts[g_currentFont];
    const bool isMonospaced = g_align & A_FONT_ALIGN_MONOSPACED;
    const char* text;

    for(text = Text; *text != '\0'; text++) {
        int charWidth = 0;
        ASprite* spr = f->sprites[(int)*text];

        if(*text == ' ') {
            charWidth = isMonospaced ? f->maxWidth : BLANK_SPACE;
        } else if(spr != NULL) {
            charWidth = isMonospaced ? f->maxWidth : spr->w;
        }

        numChars++;

        if(charWidth > 0) {
            tally += charWidth;

            if(tally > g_lineWidth) {
                break;
            }

            tally += CHAR_SPACING;
        }
    }

    if(*text == '\0') {
        // No need to shorten text
        return false;
    }

    const int dotsWidth = a_font_width("...");

    if(dotsWidth > g_lineWidth) {
        // No room for "..."
        *NewText = "\0";
        return false;
    }

    tally += CHAR_SPACING + dotsWidth;

    for(const char* text = Text + numChars - 1; tally > g_lineWidth; text--) {
        int charWidth = 0;
        ASprite* spr = f->sprites[(int)*text];

        if(*text == ' ') {
            charWidth = isMonospaced ? f->maxWidth : BLANK_SPACE;
        } else if(spr != NULL) {
            charWidth = isMonospaced ? f->maxWidth : spr->w;
        }

        numChars--;

        if(charWidth > 0) {
            tally -= CHAR_SPACING + charWidth;
        }
    }

    char* buffer = a_mem_malloc((numChars + 4) * sizeof(char));
    memcpy(buffer, Text, numChars * sizeof(char));

    buffer[numChars + 0] = '.';
    buffer[numChars + 1] = '.';
    buffer[numChars + 2] = '.';
    buffer[numChars + 3] = '\0';

    *NewText = buffer;
    return true;
}

void a_font__init(void)
{
    g_fontsList = a_list_new();
    g_fonts = NULL;
    g_currentFont = 0;
    g_align = A_FONT_ALIGN_LEFT;
    g_x = g_initialX = 0;
    g_y = 0;
    g_lineWidth = 0;

    ASprite* const fontSprite = a_sprite_fromData(g_media_font);

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
    const AFont* const src = g_fonts[Font];
    AFont* const f = a_mem_malloc(sizeof(AFont));

    for(int i = NUM_ASCII; i--; ) {
        if(src->sprites[i]) {
            f->sprites[i] = a_sprite_clone(src->sprites[i]);

            ASprite* s = f->sprites[i];
            APixel* d = s->pixels;

            for(int j = s->w * s->h; j--; d++) {
                if(*d != A_SPRITE_COLORKEY) {
                    *d = Color;
                }
            }
        } else {
            f->sprites[i] = NULL;
        }
    }

    f->maxWidth = src->maxWidth;
    f->maxHeight = src->maxHeight;

    a_list_addLast(g_fontsList, f);

    free(g_fonts);
    g_fonts = (AFont**)a_list_array(g_fontsList);

    return a_list_size(g_fontsList) - 1;
}

void a_font_setFace(int Font)
{
    g_currentFont = Font;
}

void a_font_setAlign(AFontAlign Align)
{
    g_align = Align;
}

void a_font_setCoords(int X, int Y)
{
    g_x = g_initialX = X;
    g_y = Y;
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
    g_y += g_fonts[g_currentFont]->maxHeight + LINE_SPACING;
}

int a_font_lineHeight(void)
{
    return g_fonts[g_currentFont]->maxHeight + LINE_SPACING;
}

void a_font_setLineWidth(int LineWidth)
{
    g_lineWidth = LineWidth;
}

void a_font_resetLineWidth(void)
{
    g_lineWidth = 0;
}

void a_font_text(const char* Text)
{
    if(*Text == '\0') {
        return;
    }

    char* newBuffer = NULL;
    bool freeNewBuffer = false;
    const bool isMonospaced = g_align & A_FONT_ALIGN_MONOSPACED;

    if(g_lineWidth > 0) {
        freeNewBuffer = shortenText(Text, &newBuffer);

        if(newBuffer != NULL) {
            Text = newBuffer;
        }
    }

    if(g_align & A_FONT_ALIGN_MIDDLE) {
        g_x -= a_font_width(Text) / 2;
    }

    if(g_align & A_FONT_ALIGN_RIGHT) {
        g_x -= a_font_width(Text);
    }

    const AFont* const f = g_fonts[g_currentFont];

    for( ; *Text != '\0'; Text++) {
        ASprite* spr = f->sprites[(int)*Text];

        if(spr) {
            if(isMonospaced) {
                a_sprite_blit(spr, g_x + (f->maxWidth - spr->w) / 2, g_y);
                g_x += f->maxWidth + CHAR_SPACING;
            } else {
                a_sprite_blit(spr, g_x, g_y);
                g_x += spr->w + CHAR_SPACING;
            }
        } else if(*Text == ' ') {
            g_x += (isMonospaced ? f->maxWidth : BLANK_SPACE) + CHAR_SPACING;
        }
    }

    if(freeNewBuffer) {
        free(newBuffer);
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

void a_font_int(int Number)
{
    char s[21];
    snprintf(s, 21, "%d", Number);

    a_font_text(s);
}

void a_font_float(float Number)
{
    char s[64];
    snprintf(s, 64, "%f", Number);

    a_font_text(s);
}

void a_font_double(double Number)
{
    char s[64];
    snprintf(s, 64, "%lf", Number);

    a_font_text(s);
}

void a_font_char(char Character)
{
    char s[2];
    snprintf(s, 2, "%c", Character);

    a_font_text(s);
}

int a_font_width(const char* Text)
{
    int width = 0;
    AFont* const f = g_fonts[g_currentFont];
    const bool isMonospaced = g_align & A_FONT_ALIGN_MONOSPACED;

    if(*Text == '\0') {
        return 0;
    }

    for( ; *Text != '\0'; Text++) {
        ASprite* spr = f->sprites[(int)*Text];

        if(spr) {
            width += (isMonospaced ? f->maxWidth : spr->w) + CHAR_SPACING;
        } else if(*Text == ' ') {
            width += (isMonospaced ? f->maxWidth : BLANK_SPACE) + CHAR_SPACING;
        }
    }

    return width - CHAR_SPACING;
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
