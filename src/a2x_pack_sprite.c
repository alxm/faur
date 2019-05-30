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

static ASprite* spriteNew(APixels* Pixels, int X, int Y, int FrameWidth, int FrameHeight)
{
    AVectorInt gridDim;

    if(X == 0 && Y == 0 && FrameWidth < 1 && FrameHeight < 1) {
        gridDim = (AVectorInt){Pixels->w, Pixels->h};
    } else {
        gridDim = a_pixels__boundsFind(Pixels, X, Y);
    }

    if(FrameWidth < 1 || FrameHeight < 1) {
        FrameWidth = gridDim.x;
        FrameHeight = gridDim.y;
    }

    unsigned framesNum =
        (unsigned)((gridDim.x / FrameWidth) * (gridDim.y / FrameHeight));

    int endX = X + gridDim.x - (gridDim.x % FrameWidth);
    int endY = Y + gridDim.y - (gridDim.y % FrameHeight);

    ASprite* s = a_mem_malloc(sizeof(ASprite) + sizeof(APixels*) * framesNum);

    s->nameId = NULL;
    s->framesNum = framesNum;

    unsigned frame = 0;

    for(int y = Y; y < endY; y += FrameHeight) {
        for(int x = X; x < endX; x += FrameWidth) {
            s->pixels[frame] =
                a_pixels__sub(Pixels, x, y, FrameWidth, FrameHeight);

            a_pixels__commit(s->pixels[frame]);

            frame++;
        }
    }

    return s;
}

ASprite* a_sprite_newFromPng(const char* Path, int X, int Y, int FrameWidth, int FrameHeight)
{
    APixels* pixels = a_png__readFile(Path);

    if(pixels == NULL) {
        A__FATAL("a_sprite_newFromPng(%s): Cannot read file", Path);
    }

    if(FrameWidth < 1 || FrameHeight < 1) {
        char* suffix = a_str_suffixGetFromLast(Path, '_');

        if(suffix) {
            int n = sscanf(suffix, "grid%dx%d.png", &FrameWidth, &FrameHeight);

            if(n != 2) {
                FrameWidth = 0;
                FrameHeight = 0;
            }

            free(suffix);
        }
    }

    ASprite* s = spriteNew(pixels, X, Y, FrameWidth, FrameHeight);

    s->nameId = a_str_dup(Path);

    a_pixels__free(pixels);

    return s;
}

ASprite* a_sprite_newFromSprite(const ASprite* Sheet, int X, int Y, int FrameWidth, int FrameHeight)
{
    return spriteNew(Sheet->pixels[0], X, Y, FrameWidth, FrameHeight);
}

ASprite* a_sprite_newBlank(int Width, int Height, unsigned Frames, bool ColorKeyed)
{
    if(Frames == 0) {
        A__FATAL("a_sprite_newBlank: Frames == 0");
    }

    ASprite* s = a_mem_malloc(sizeof(ASprite) + sizeof(APixels*) * Frames);

    s->nameId = NULL;
    s->framesNum = Frames;

    for(unsigned f = Frames; f--; ) {
        s->pixels[f] = a_pixels__new(Width, Height, true, true);

        if(ColorKeyed) {
            a_pixels__fill(s->pixels[f], a_sprite__colorKey);
        }

        a_pixels__commit(s->pixels[f]);
    }

    return s;
}

ASprite* a_sprite_dup(const ASprite* Sprite)
{
    ASprite* s = a_mem_malloc(
                    sizeof(ASprite) + sizeof(APixels*) * Sprite->framesNum);

    s->nameId = a_str_dup(Sprite->nameId);
    s->framesNum = Sprite->framesNum;

    for(unsigned f = Sprite->framesNum; f--; ) {
        s->pixels[f] = a_pixels__dup(Sprite->pixels[f]);

        a_pixels__commit(s->pixels[f]);

        #if !A_CONFIG_LIB_RENDER_SOFTWARE
            // Sprite's pixel buffer may be stale, blit the real texture
            a_color_push();
            a_screen_push((ASprite*)s, f);

            a_color_reset();
            a_sprite_blit(Sprite, f, 0, 0);

            a_screen_pop();
            a_color_pop();
        #endif
    }

    return s;
}

void a_sprite_free(ASprite* Sprite)
{
    if(Sprite == NULL) {
        return;
    }

    for(unsigned f = Sprite->framesNum; f--; ) {
        a_pixels__free(Sprite->pixels[f]);
    }

    free(Sprite->nameId);
    free(Sprite);
}

void a_sprite_blit(const ASprite* Sprite, unsigned Frame, int X, int Y)
{
    a_platform_api__textureBlit(Sprite->pixels[Frame]->texture,
                                X,
                                Y,
                                a__color.fillBlit);
}

void a_sprite_blitEx(const ASprite* Sprite, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY)
{
    a_platform_api__textureBlitEx(Sprite->pixels[Frame]->texture,
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
    for(unsigned f = Sprite->framesNum; f--; ) {
        APixel* buffer = Sprite->pixels[f]->buffer;

        for(size_t i = Sprite->pixels[f]->bufferSize / sizeof(APixel); i--; ) {
            if(buffer[i] == OldColor) {
                buffer[i] = NewColor;
            }
        }

        a_pixels__commit(Sprite->pixels[f]);
    }
}

void a_sprite_swapColors(ASprite* Sprite, const APixel* OldColors, const APixel* NewColors, unsigned NumColors)
{
    for(unsigned f = Sprite->framesNum; f--; ) {
        APixel* buffer = Sprite->pixels[f]->buffer;

        for(size_t i = Sprite->pixels[f]->bufferSize / sizeof(APixel); i--; ) {
            const APixel pixel = buffer[i];

            for(unsigned c = NumColors; c--; ) {
                if(pixel == OldColors[c]) {
                    buffer[i] = NewColors[c];
                    break;
                }
            }
        }

        a_pixels__commit(Sprite->pixels[f]);
    }
}

AVectorInt a_sprite_sizeGet(const ASprite* Sprite)
{
    return (AVectorInt){Sprite->pixels[0]->w, Sprite->pixels[0]->h};
}

int a_sprite_sizeGetWidth(const ASprite* Sprite)
{
    return Sprite->pixels[0]->w;
}

int a_sprite_sizeGetHeight(const ASprite* Sprite)
{
    return Sprite->pixels[0]->h;
}

unsigned a_sprite_framesNumGet(const ASprite* Sprite)
{
    return Sprite->framesNum;
}

const APixel* a_sprite_pixelsGetBuffer(const ASprite* Sprite, unsigned Frame)
{
    return Sprite->pixels[Frame]->buffer;
}

APixel a_sprite_pixelsGetPixel(const ASprite* Sprite, unsigned Frame, int X, int Y)
{
    return a_pixels__bufferGetAt(Sprite->pixels[Frame], X, Y);
}

APixel a_sprite_colorKeyGet(void)
{
    return a_sprite__colorKey;
}
