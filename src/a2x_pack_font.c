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

#define CHARS_NUM 93
static const char const chars[CHARS_NUM] = {
    'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g',
    'H', 'h', 'I', 'i', 'J', 'j', 'K', 'k', 'L', 'l', 'M', 'm', 'N', 'n',
    'O', 'o', 'P', 'p', 'Q', 'q', 'R', 'r', 'S', 's', 'T', 't', 'U', 'u',
    'V', 'v', 'W', 'w', 'X', 'x', 'Y', 'y', 'Z', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '_', '-', '+', '=', '*', '/', '\\', '&', '$', '@', '!', '?', '\'', '"',
    '(', ')', '[', ']', '{', '}', '.', ',', '~', ':', ';', '%', '^', '#', '<', '>', '|'
};

static List* fontsList;
static Font** fonts;

static int charIndex(const char c);

void a_font__init(void)
{
    fontsList = a_list_new();
    fonts = NULL;
}

void a_font__free(void)
{
    a_list_free(fontsList, true);
    free(fonts);
}

int a_font_load(Sheet* const sheet, const int sx, const int sy, const int zoom, const FontLoad loader)
{
    Font* const f = malloc(sizeof(Font));

    for(int i = NUM_ASCII; i--; ) {
        f->sprites[i] = NULL;
    }

    f->maxWidth = 0;

    a_list_addLast(fontsList, f);

    free(fonts);
    fonts = (Font**)a_list_getArray(fontsList);

    int x = sx;
    int y = sy;
    int y2 = sy;

    const Pixel limit = sheet->limit;

    do {
        y2++;
    } while(a_sprite__getPixel(sheet, x, y2) != limit);

    int start = 0;
    int end = CHARS_NUM - 1;

	switch(loader & (~A_LOAD_CAPS)) {
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

    for(int i = start; i <= end; i++, x++) {
        int areax = x;
        int areay = y;

        do {
            x++;
        } while(a_sprite__getPixel(sheet, x, y) != limit);

        int areaw = x - areax;
        int areah = y2 - y;

        f->sprites[(int)chars[i]] = a_sprite_newZoomed(sheet, areax, areay, areaw, areah, zoom);

        if(((loader & A_LOAD_N) && chars[i] == '0') || chars[i] == 'A') {
            f->maxWidth = areaw;
        }

        if((loader & A_LOAD_CAPS) && isalpha(chars[i])) {
            f->sprites[(int)chars[i + 1]] = f->sprites[(int)chars[i]];
            i++;
        }
    }

    return a_list_size(fontsList) - 1;
}

int a_font_copy(const int font, const uint8_t r, const uint8_t g, const uint8_t b)
{
    const Font* const src = fonts[font];
    Font* const f = malloc(sizeof(Font));

    const Pixel colour = a_pixel_make(r, g, b);

    for(int i = NUM_ASCII; i--; ) {
        if(src->sprites[i]) {
            f->sprites[i] = a_sprite_clone(src->sprites[i]);

            Sprite* const s = f->sprites[i];
            const Pixel t = s->t;
            Pixel* d = s->data;

            for(int j = s->w * s->h; j--; d++) {
                if(*d != t) {
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
    fonts = (Font**)a_list_getArray(fontsList);

    return a_list_size(fontsList) - 1;
}

int a_font_text(const FontAlign align, int x, const int y, const int index, const char* const text)
{
    if(align & A_SAFE) {
        return a_font_safe(align, x, y, index, text);
    }

    Font* const f = fonts[index];

    int spaced = 0;

    if(align & A_SPACED) spaced = 1;
    if(align & A_MIDDLE) x -= a_font_width(index, text) / 2;
    if(align & A_RIGHT) x -= a_font_width(index, text);

    const int length = strlen(text);
    const int maxWidth = f->maxWidth;
    int drx = x;

    for(int l = 0; l < length; l++) {
        int drx2 = drx;

        if(text[l] == ' ') {
            drx += FONT_SPACE + FONT_BLANK_SPACE;
        } else {
            const int index = (int)text[l];
            Sprite* const spr = f->sprites[index];

            if(index < 0 || index >= NUM_ASCII) {
                continue;
            }

            if(spaced) {
                drx2 += (f->maxWidth - spr->w) / 2;
            }

            a_blit(spr, drx2, y);
            drx += FONT_SPACE + (spaced ? maxWidth : spr->w);
        }
    }

    return drx;
}

int a_font_safe(const FontAlign align, int x, const int y, const int index, const char* const text)
{
    Font* const f = fonts[index];

    int spaced = 0;

    if(align & A_SPACED) spaced = 1;
    if(align & A_MIDDLE) x -= a_font_width(index, text) / 2;
    if(align & A_RIGHT) x -= a_font_width(index, text);

    const int length = strlen(text);
    const int maxWidth = f->maxWidth;
    int drx = x;

    for(int l = 0; l < length; l++) {
        int drx2 = drx;

        if(text[l] == ' ') {
            drx += FONT_SPACE + FONT_BLANK_SPACE;
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
                drx2 += (f->maxWidth - spr->w) / 2;
            }

            a_blit(spr, drx2, y);
            drx += FONT_SPACE + (spaced ? maxWidth : spr->w);
        }
    }

    return drx;
}

int a_font_int(const FontAlign align, const int x, const int y, const int f, const int number)
{
    char s[16];
    sprintf(s, "%d", number);

    return a_font_text(align, x, y, f, s);
}

int a_font_float(const FontAlign align, const int x, const int y, const int f, const float number)
{
    char s[64];
    sprintf(s, "%f", number);

    return a_font_text(align, x, y, f, s);
}

int a_font_char(const FontAlign align, const int x, const int y, const int f, const char ch)
{
    char s[2];
    sprintf(s, "%c", ch);

    return a_font_text(align, x, y, f, s);
}

int a_font_fixed(const FontAlign align, const int x, const int y, const int f, const int width, const char* const text)
{
    if(a_font_width(f, text) <= width) {
        return a_font_text(align, x, y, f, text);
    }

    const int w = a_font_width(f, "...");
    char* const temp = a_str_dup(text);

    do {
        temp[strlen(temp) - 1] = '\0';
    } while(a_font_width(f, temp) + w > width);

    int rx = a_font_textf(align, x, y, f, "%s...", temp);

    return rx;
}

int a_font_width(const int index, const char* const text)
{
    Font* const f = fonts[index];

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

static int charIndex(const char c)
{
    for(int i = 0; i < CHARS_NUM; i++) {
        if(chars[i] == c) return i;
    }

    return -1;
}
