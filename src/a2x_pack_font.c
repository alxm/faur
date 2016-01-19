/*
    Copyright 2010 Alex Margarit

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

#define NUM_ASCII 256

#define FONT_SPACE       1
#define FONT_BLANK_SPACE 3

typedef struct Font {
    Sprite* sprites[NUM_ASCII];
    int maxWidth;
} Font;

static const char chars[] =
    "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
    "0123456789"
    "_-+=*/\\&$@!?'\"()[]{}.,~:;%^#<>|`";

#define CHARS_NUM (sizeof(chars) / sizeof(char) - 1)

static List* fontsList;
static Font** fonts;
static int font;
static FontAlign align;
static int x;
static int y;

static int charIndex(char c);

void a_font__init(void)
{
    fontsList = a_list_new();
    fonts = NULL;
    font = 0;
    align = A_LEFT;
    x = 0;
    y = 0;
}

void a_font__uninit(void)
{
    A_LIST_ITERATE(fontsList, Font, f) {
        free(f);
    }
    a_list_free(fontsList);
    free(fonts);
}

int a_font_load(const Sprite* sheet, int x, int y, int zoom, FontLoad loader)
{
    Font* const f = malloc(sizeof(Font));

    for(int i = NUM_ASCII; i--; ) {
        f->sprites[i] = NULL;
    }

    f->maxWidth = 0;

    a_list_addLast(fontsList, f);

    free(fonts);
    fonts = (Font**)a_list_array(fontsList);

    int start = 0;
    int end = CHARS_NUM - 1;

	switch(loader & ~A_LOAD_CAPS) {
		case A_LOAD_AN: {
            end = charIndex('9');
		} break;

		case A_LOAD_A: {
            end = charIndex('z');
		} break;

		case A_LOAD_N: {
			start = charIndex('0');
            end = charIndex('9');
		} break;
	}

    SpriteFrames* const sf = a_spriteframes_new(sheet, x, y, 0);

    for(int i = start; i <= end; i++) {
        f->sprites[(int)chars[i]] = a_spriteframes_next(sf);

        if((loader & A_LOAD_CAPS) && isalpha(chars[i])) {
            f->sprites[(int)chars[i + 1]] = f->sprites[(int)chars[i]];
            i++;
        }
    }

    a_spriteframes_free(sf);

    return a_list_size(fontsList) - 1;
}

int a_font_copy(int font, uint8_t r, uint8_t g, uint8_t b)
{
    const Font* const src = fonts[font];
    Font* const f = malloc(sizeof(Font));

    const Pixel colour = a_pixel_make(r, g, b);

    for(int i = NUM_ASCII; i--; ) {
        if(src->sprites[i]) {
            f->sprites[i] = a_sprite_clone(src->sprites[i]);

            Sprite* const s = f->sprites[i];
            Pixel* d = s->data;

            for(int j = s->w * s->h; j--; d++) {
                if(*d != A_SPRITE_TRANSPARENT) {
                    *d = colour;
                }
            }
        } else {
            f->sprites[i] = NULL;
        }
    }

    f->maxWidth = src->maxWidth;

    a_list_addLast(fontsList, f);

    free(fonts);
    fonts = (Font**)a_list_array(fontsList);

    return a_list_size(fontsList) - 1;
}

void a_font_setFace(int f)
{
    font = f;
}

void a_font_setAlign(FontAlign a)
{
    align = a;
}

void a_font_setCoords(int X, int Y)
{
    x = X;
    y = Y;
}

int a_font_getX(void)
{
    return x;
}

void a_font_text(const char* text)
{
    Font* const f = fonts[font];
    bool spaced = align & A_SPACED;

    if(align & A_MIDDLE) {
        x -= a_font_width(text) / 2;
    }

    if(align & A_RIGHT) {
        x -= a_font_width(text);
    }

    const int length = strlen(text);
    const int maxWidth = f->maxWidth;

    for(int l = 0; l < length; l++) {
        if(text[l] == ' ') {
            x += FONT_SPACE + FONT_BLANK_SPACE;
        } else {
            const int index = (int)text[l];

            if(index < 0 || index >= NUM_ASCII) {
                continue;
            }

            Sprite* const spr = f->sprites[index];

            if(!spr) {
                continue;
            }

            if(spaced) {
                a_blit(spr, x + (f->maxWidth - spr->w) / 2, y);
            } else {
                a_blit(spr, x, y);
            }

            x += FONT_SPACE + (spaced ? maxWidth : spr->w);
        }
    }
}

void a_font_int(int number)
{
    char s[21];
    sprintf(s, "%d", number);

    a_font_text(s);
}

void a_font_float(float number)
{
    char s[64];
    sprintf(s, "%f", number);

    a_font_text(s);
}

void a_font_double(double number)
{
    char s[64];
    sprintf(s, "%lf", number);

    a_font_text(s);
}

void a_font_char(char ch)
{
    char s[2];
    sprintf(s, "%c", ch);

    a_font_text(s);
}

void a_font_fixed(int width, const char* text)
{
    if(a_font_width(text) <= width) {
        a_font_text(text);
        return;
    }

    const int w = a_font_width("...");
    char* const temp = a_str_dup(text);

    do {
        temp[strlen(temp) - 1] = '\0';
    } while(a_font_width(temp) + w > width);

    a_font_textf("%s...", temp);
}

int a_font_width(const char* text)
{
    Font* const f = fonts[font];

    const int length = strlen(text);

    int x = 0;

    for(int l = 0; l < length; l++) {
        if(text[l] == ' ') {
            x += FONT_SPACE + FONT_BLANK_SPACE;
        } else {
            const int i = (int)text[l];

            if(i >= 0 && i < NUM_ASCII) {
                x += FONT_SPACE + f->sprites[i]->w;
            }
        }
    }

    return x - FONT_SPACE;
}

static int charIndex(char c)
{
    for(int i = 0; i < CHARS_NUM; i++) {
        if(chars[i] == c) return i;
    }

    return -1;
}
