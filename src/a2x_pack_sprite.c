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

#include "a2x_pack_sprite.v.h"

#include "a2x_pack_embed.v.h"
#include "a2x_pack_file.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_png.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_str.v.h"

APixel a_sprite__colorKey;
APixel a_sprite__colorLimit;
APixel a_sprite__colorEnd;

void a_sprite__init(void)
{
    a_sprite__colorKey = a_settings_pixelGet(A_SETTING_COLOR_KEY);
    a_sprite__colorLimit = a_settings_pixelGet(A_SETTING_COLOR_LIMIT);
    a_sprite__colorEnd = a_settings_pixelGet(A_SETTING_COLOR_END);
}

static ASprite* makeEmptySprite(int Width, int Height)
{
    ASprite* s = a_mem_malloc(sizeof(ASprite));

    s->pixels = NULL;
    s->pixelsSize = (unsigned)Width * (unsigned)Height * sizeof(APixel);
    s->nameId = NULL;
    s->w = Width;
    s->wOriginal = Width;
    s->wLog2 = (int)log2f((float)Width);
    s->h = Height;
    s->texture = NULL;

    return s;
}

static void assignPixels(ASprite* Sprite, APixel* Pixels)
{
    free(Sprite->pixels);
    Sprite->pixels = Pixels;

    a_platform__textureSpriteCommit(Sprite);
}

ASprite* a_sprite_newFromFile(const char* Path)
{
    int w = 0;
    int h = 0;
    APixel* pixels = NULL;

    if(a_file_exists(Path)) {
        a_png_readFile(Path, &pixels, &w, &h);
    } else {
        const uint8_t* data = NULL;

        if(a_embed__get(Path, &data, NULL)) {
            a_png_readMemory(data, &pixels, &w, &h);
        }
    }

    if(pixels == NULL) {
        return NULL;
    }

    ASprite* s = makeEmptySprite(w, h);

    assignPixels(s, pixels);
    s->nameId = a_str_dup(Path);

    return s;
}

ASprite* a_sprite_newFromSprite(const ASprite* Sheet, int X, int Y)
{
    int spriteWidth = 0;
    int spriteHeight = 0;
    const int sheetWidth = Sheet->w;
    const int sheetHeight = Sheet->h;

    if(X < 0 || X >= Sheet->w || Y < 0 || Y >= Sheet->h) {
        a_out__fatal("a_sprite_newFromSprite: %s invalid coords %d, %d",
                     A_SPRITE__NAME(Sheet),
                     X,
                     Y);
    }

    for(int endX = X; endX < sheetWidth; endX++) {
        APixel p = a_sprite__pixelsGetPixel(Sheet, endX, Y);

        if(p != a_sprite__colorLimit && p != a_sprite__colorEnd) {
            continue;
        }

        for(int endY = Y; endY < sheetHeight; endY++) {
            p = a_sprite__pixelsGetPixel(Sheet, X, endY);

            if(p != a_sprite__colorLimit) {
                continue;
            }

            bool foundRightEdge = true;
            bool foundBottomEdge = true;

            for(int x = X; x < endX; x++) {
                p = a_sprite__pixelsGetPixel(Sheet, x, endY);

                if(p != a_sprite__colorLimit) {
                    foundBottomEdge = false;
                    break;
                }
            }

            if(foundBottomEdge) {
                for(int y = Y; y < endY; y++) {
                    p = a_sprite__pixelsGetPixel(Sheet, endX, y);

                    if(p != a_sprite__colorLimit && p != a_sprite__colorEnd) {
                        foundRightEdge = false;
                        break;
                    }
                }

                if(foundRightEdge) {
                    spriteWidth = endX - X;
                    spriteHeight = endY - Y;
                    goto doneEdges;
                }
            }
        }
    }

doneEdges:
    if(spriteWidth == 0 || spriteHeight == 0) {
        if(X == 0 && Y == 0) {
            // no boundary borders for full-image sprites
            spriteWidth = sheetWidth;
            spriteHeight = sheetHeight;
        } else {
            a_out__fatal("a_sprite_newFromSprite: %s invalid coords %d, %d",
                         A_SPRITE__NAME(Sheet),
                         X,
                         Y);
        }
    }

    ASprite* sprite = makeEmptySprite(spriteWidth, spriteHeight);
    APixel* pixels = a_mem_malloc(sprite->pixelsSize);

    const APixel* src = Sheet->pixels + Y * sheetWidth + X;
    APixel* dst = pixels;

    for(int i = spriteHeight; i--; ) {
        memcpy(dst, src, (unsigned)spriteWidth * sizeof(APixel));
        src += sheetWidth;
        dst += spriteWidth;
    }

    assignPixels(sprite, pixels);

    return sprite;
}

ASprite* a_sprite_newBlank(int Width, int Height, bool ColorKeyed)
{
    ASprite* s = makeEmptySprite(Width, Height);
    APixel* pixels;

    if(ColorKeyed) {
        pixels = a_mem_malloc(s->pixelsSize);
        APixel* p = pixels;

        for(int i = Width * Height; i--; ) {
            *p++ = a_sprite__colorKey;
        }
    } else {
        pixels = a_mem_zalloc(s->pixelsSize);
    }

    assignPixels(s, pixels);

    return s;
}

ASprite* a_sprite_dup(const ASprite* Sprite)
{
    ASprite* clone = makeEmptySprite(Sprite->w, Sprite->h);
    APixel* pixels = a_mem_dup(Sprite->pixels, Sprite->pixelsSize);

    assignPixels(clone, pixels);

    #if !A_BUILD_RENDER_SOFTWARE
        a_pixel_push();
        a_screen_targetPushSprite(clone);

        a_pixel_reset();
        a_sprite_blit(Sprite, 0, 0);

        a_screen_targetPop();
        a_pixel_pop();
    #endif

    return clone;
}

void a_sprite_free(ASprite* Sprite)
{
    if(Sprite == NULL) {
        return;
    }

    a_platform__textureFree(Sprite->texture);

    free(Sprite->nameId);
    free(Sprite->pixels);
    free(Sprite);
}

void a_sprite_blit(const ASprite* Sprite, int X, int Y)
{
    a_platform__textureBlit(Sprite->texture, X, Y, a_pixel__state.fillBlit);
}

void a_sprite_blitCenter(const ASprite* Sprite)
{
    a_sprite_blit(Sprite,
                  (a__screen.width - Sprite->w) / 2,
                  (a__screen.height - Sprite->h) / 2);
}

void a_sprite_blitCenterX(const ASprite* Sprite, int Y)
{
    a_sprite_blit(Sprite,
                  (a__screen.width - Sprite->w) / 2,
                  Y);
}

void a_sprite_blitCenterY(const ASprite* Sprite, int X)
{
    a_sprite_blit(Sprite,
                  X,
                  (a__screen.height - Sprite->h) / 2);
}

void a_sprite_blitEx(const ASprite* Sprite, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY)
{
    a_platform__textureBlitEx(Sprite->texture,
                              X,
                              Y,
                              Scale,
                              a_fix_wrapAngleInt(Angle),
                              CenterX,
                              CenterY,
                              a_pixel__state.fillBlit);
}

void a_sprite_swapColor(ASprite* Sprite, APixel OldColor, APixel NewColor)
{
    for(size_t i = Sprite->pixelsSize / sizeof(APixel); i--; ) {
        if(Sprite->pixels[i] == OldColor) {
            Sprite->pixels[i] = NewColor;
        }
    }

    a_platform__textureSpriteCommit(Sprite);
}

void a_sprite_swapColors(ASprite* Sprite, APixel* OldColors, APixel* NewColors, unsigned NumColors)
{
    for(size_t i = Sprite->pixelsSize / sizeof(APixel); i--; ) {
        const APixel pixel = Sprite->pixels[i];

        for(unsigned c = NumColors; c--; ) {
            if(pixel == OldColors[c]) {
                Sprite->pixels[i] = NewColors[c];
                break;
            }
        }
    }

    a_platform__textureSpriteCommit(Sprite);
}

int a_sprite_widthGet(const ASprite* Sprite)
{
    return Sprite->w;
}

int a_sprite_widthGetLog2(const ASprite* Sprite)
{
    return Sprite->wLog2;
}

int a_sprite_widthGetOriginal(const ASprite* Sprite)
{
    return Sprite->wOriginal;
}

void a_sprite_widthSetPowerOf2(ASprite* Sprite)
{
    if((Sprite->w & (Sprite->w - 1)) == 0) {
        return;
    }

    int power = 1;

    while((1 << power) < Sprite->w) {
        power++;
    }

    int newWidth = 1 << power;
    size_t newSize = (unsigned)newWidth * (unsigned)Sprite->h * sizeof(APixel);
    APixel* newPixels = a_mem_malloc(newSize);

    int oldWidth = Sprite->w;
    size_t oldLineSize = (unsigned)oldWidth * sizeof(APixel);
    APixel* oldPixels = Sprite->pixels;

    int leftPadding = (newWidth - oldWidth) / 2;
    int rightPadding = newWidth - oldWidth - leftPadding;

    for(int i = Sprite->h; i--; ) {
        for(int j = leftPadding; j--; ) {
            *newPixels++ = a_sprite__colorKey;
        }

        memcpy(newPixels, oldPixels, oldLineSize);
        newPixels += oldWidth;
        oldPixels += oldWidth;

        for(int j = rightPadding; j--; ) {
            *newPixels++ = a_sprite__colorKey;
        }
    }

    Sprite->w = newWidth;
    Sprite->wLog2 = power;
    Sprite->pixelsSize = newSize;

    assignPixels(Sprite, newPixels - newSize / sizeof(APixel));
}

int a_sprite_heightGet(const ASprite* Sprite)
{
    return Sprite->h;
}

APixel* a_sprite_pixelsGetBuffer(ASprite* Sprite)
{
    return Sprite->pixels;
}

APixel a_sprite_pixelsGetPixel(const ASprite* Sprite, int X, int Y)
{
    return *(Sprite->pixels + Y * Sprite->w + X);
}

APixel a_sprite_colorKeyGet(void)
{
    return a_sprite__colorKey;
}
