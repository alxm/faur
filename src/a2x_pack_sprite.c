/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_sprite.v.h"

#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_png.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_str.v.h"

APixel a_sprite__colorKey;
APixel a_sprite__colorLimit;

void a_sprite__init(void)
{
    a_sprite__colorKey = a_pixel_fromHex(A_CONFIG_COLOR_SPRITE_KEY);
    a_sprite__colorLimit = a_pixel_fromHex(A_CONFIG_COLOR_SPRITE_BORDER);
}

static ASprite* spriteNew(APixels* Pixels)
{
    ASprite* s = a_mem_malloc(sizeof(ASprite));

    s->pixels = Pixels;
    s->nameId = NULL;
    s->wOriginal = Pixels->w;
    s->wLog2 = (int)log2f((float)Pixels->w);

    return s;
}

static int findNextVerticalEdge(const ASprite* Sheet, int StartX, int StartY, int* EdgeX)
{
    int w = Sheet->pixels->w;
    int h = Sheet->pixels->h;

    for(int x = StartX + *EdgeX + 1; x < w; x++) {
        APixel p = a_pixels__bufferGetAt(Sheet->pixels, x, StartY);

        if(p == a_sprite__colorLimit) {
            *EdgeX = x - StartX;

            int len = 1;
            APixel* buffer = a_pixels__bufferGetFrom(
                                Sheet->pixels, x, StartY + 1);

            for(int y = h - (StartY + 1); y--; ) {
                if(*buffer != a_sprite__colorLimit) {
                    break;
                }

                buffer += w;
                len++;
            }

            return len;
        }
    }

    return -1;
}

static int findNextHorizontalEdge(const ASprite* Sheet, int StartX, int StartY, int* EdgeY)
{
    for(int y = StartY + *EdgeY + 1; y < Sheet->pixels->h; y++) {
        APixel p = a_pixels__bufferGetAt(Sheet->pixels, StartX, y);

        if(p == a_sprite__colorLimit) {
            *EdgeY = y - StartY;

            int len = 1;
            APixel* buffer = a_pixels__bufferGetFrom(
                                Sheet->pixels, StartX + 1, y);

            for(int x = Sheet->pixels->w - (StartX + 1); x--; ) {
                if(*buffer != a_sprite__colorLimit) {
                    break;
                }

                buffer++;
                len++;
            }

            return len;
        }
    }

    return -1;
}

AVectorInt a_sprite__boundsFind(const ASprite* Sheet, int X, int Y)
{
    AVectorInt bounds;

    if(X < 0 || X >= Sheet->pixels->w || Y < 0 || Y >= Sheet->pixels->h) {
        A__FATAL("a_sprite__boundsFind(%s, %d, %d): Invalid coords",
                 A_SPRITE__NAME(Sheet),
                 X,
                 Y);
    }

    int vEdgeX = 0;
    int vEdgeLen = findNextVerticalEdge(Sheet, X, Y, &vEdgeX);
    int hEdgeY = 0;
    int hEdgeLen = findNextHorizontalEdge(Sheet, X, Y, &hEdgeY);

    while(vEdgeLen != -1 && hEdgeLen != -1) {
        if(vEdgeLen < hEdgeY) {
            vEdgeLen = findNextVerticalEdge(Sheet, X, Y, &vEdgeX);
        } else if(hEdgeLen < vEdgeX) {
            hEdgeLen = findNextHorizontalEdge(Sheet, X, Y, &hEdgeY);
        } else {
            break;
        }
    }

    if(vEdgeLen == -1 || hEdgeLen == -1) {
        bounds.x = Sheet->pixels->w - X;
        bounds.y = Sheet->pixels->h - Y;
    } else {
        bounds.x = vEdgeX;
        bounds.y = hEdgeY;
    }

    return bounds;
}

ASprite* a_sprite_newFromPng(const char* Path)
{
    APixels* pixels = a_png__readFile(Path);

    if(pixels == NULL) {
        A__FATAL("a_sprite_newFromPng(%s): Cannot read file", Path);
    }

    ASprite* s = spriteNew(pixels);

    s->nameId = a_str_dup(Path);

    a_pixels__commit(s->pixels);

    return s;
}

ASprite* a_sprite_newFromSprite(const ASprite* Sheet, int X, int Y)
{
    AVectorInt dim = a_sprite__boundsFind(Sheet, X, Y);

    return a_sprite_newFromSpriteEx(Sheet, X, Y, dim.x, dim.y);
}

ASprite* a_sprite_newFromSpriteEx(const ASprite* Sheet, int X, int Y, int W, int H)
{
    ASprite* s = spriteNew(a_pixels__new(W, H));

    const APixel* src = a_pixels__bufferGetFrom(Sheet->pixels, X, Y);
    APixel* dst = s->pixels->buffer;

    for(int i = H; i--; ) {
        memcpy(dst, src, (unsigned)W * sizeof(APixel));

        src += Sheet->pixels->w;
        dst += W;
    }

    a_pixels__commit(s->pixels);

    return s;
}

ASprite* a_sprite_newBlank(int Width, int Height, bool ColorKeyed)
{
    ASprite* s = spriteNew(a_pixels__new(Width, Height));

    if(ColorKeyed) {
        a_pixels__fill(s->pixels, a_sprite__colorKey);
    }

    a_pixels__commit(s->pixels);

    return s;
}

ASprite* a_sprite_dup(const ASprite* Sprite)
{
    ASprite* clone = spriteNew(a_pixels__dup(Sprite->pixels));

    a_pixels__commit(clone->pixels);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_color_push();
        a_screen_targetPush(clone);

        a_color_reset();
        a_sprite_blit(Sprite, 0, 0);

        a_screen_targetPop();
        a_color_pop();
    #endif

    return clone;
}

void a_sprite_free(ASprite* Sprite)
{
    if(Sprite == NULL) {
        return;
    }

    a_pixels__free(Sprite->pixels);

    free(Sprite->nameId);
    free(Sprite);
}

void a_sprite_blit(const ASprite* Sprite, int X, int Y)
{
    a_platform_api__textureBlit(Sprite->pixels->texture,
                                X,
                                Y,
                                a__color.fillBlit);
}

void a_sprite_blitEx(const ASprite* Sprite, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY)
{
    a_platform_api__textureBlitEx(Sprite->pixels->texture,
                                  X,
                                  Y,
                                  Scale,
                                  a_fix_angleWrap(Angle),
                                  CenterX,
                                  CenterY,
                                  a__color.fillBlit);
}

void a_sprite_swapColor(ASprite* Sprite, APixel OldColor, APixel NewColor)
{
    APixel* buffer = Sprite->pixels->buffer;

    for(size_t i = Sprite->pixels->bufferSize / sizeof(APixel); i--; ) {
        if(buffer[i] == OldColor) {
            buffer[i] = NewColor;
        }
    }

    a_pixels__commit(Sprite->pixels);
}

void a_sprite_swapColors(ASprite* Sprite, const APixel* OldColors, const APixel* NewColors, unsigned NumColors)
{
    APixel* buffer = Sprite->pixels->buffer;

    for(size_t i = Sprite->pixels->bufferSize / sizeof(APixel); i--; ) {
        const APixel pixel = buffer[i];

        for(unsigned c = NumColors; c--; ) {
            if(pixel == OldColors[c]) {
                buffer[i] = NewColors[c];
                break;
            }
        }
    }

    a_pixels__commit(Sprite->pixels);
}

AVectorInt a_sprite_sizeGet(const ASprite* Sprite)
{
    return (AVectorInt){Sprite->pixels->w, Sprite->pixels->h};
}

int a_sprite_sizeGetWidth(const ASprite* Sprite)
{
    return Sprite->pixels->w;
}

int a_sprite_sizeGetWidthLog2(const ASprite* Sprite)
{
    return Sprite->wLog2;
}

int a_sprite_sizeGetWidthOriginal(const ASprite* Sprite)
{
    return Sprite->wOriginal;
}

int a_sprite_sizeGetHeight(const ASprite* Sprite)
{
    return Sprite->pixels->h;
}

void a_sprite_sizeSetWidthPow2(ASprite* Sprite)
{
    if((Sprite->pixels->w & (Sprite->pixels->w - 1)) == 0) {
        return;
    }

    int power = 1;

    while((1 << power) < Sprite->pixels->w) {
        power++;
    }

    int newWidth = 1 << power;
    APixels* newPixels = a_pixels__new(newWidth, Sprite->pixels->h);
    APixel* newBuffer = newPixels->buffer;

    int oldWidth = Sprite->pixels->w;
    size_t oldLineSize = (unsigned)oldWidth * sizeof(APixel);
    APixel* oldBuffer = Sprite->pixels->buffer;

    int leftPadding = (newWidth - oldWidth) / 2;
    int rightPadding = newWidth - oldWidth - leftPadding;

    for(int i = Sprite->pixels->h; i--; ) {
        for(int j = leftPadding; j--; ) {
            *newBuffer++ = a_sprite__colorKey;
        }

        memcpy(newBuffer, oldBuffer, oldLineSize);

        newBuffer += oldWidth;
        oldBuffer += oldWidth;

        for(int j = rightPadding; j--; ) {
            *newBuffer++ = a_sprite__colorKey;
        }
    }

    Sprite->wLog2 = power;

    a_pixels__free(Sprite->pixels);
    Sprite->pixels = newPixels;

    a_pixels__commit(Sprite->pixels);
}

const APixel* a_sprite_pixelsGetBuffer(const ASprite* Sprite)
{
    return Sprite->pixels->buffer;
}

APixel a_sprite_pixelsGetPixel(const ASprite* Sprite, int X, int Y)
{
    return a_pixels__bufferGetAt(Sprite->pixels, X, Y);
}

APixel a_sprite_colorKeyGet(void)
{
    return a_sprite__colorKey;
}
