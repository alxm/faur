/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_sprite.v.h"
#include <faur.v.h>

static inline bool isConstSprite(const FSprite* Sprite)
{
    return F_FLAGS_TEST_ANY(Sprite->pixels.flags, F_PIXELS__CONST);
}

const FPlatformTexture* getSpriteTexture(const FSprite* Sprite)
{
    #if F_CONFIG_SCREEN_RENDER == F_SCREEN_RENDER_SOFTWARE
        return Sprite->u.textureConst;
    #else
        return isConstSprite(Sprite)
                ? *(Sprite->u.textureIndirect) : Sprite->u.textureConst;
    #endif
}

#if F_CONFIG_SCREEN_RENDER != F_SCREEN_RENDER_SOFTWARE
static inline void lazyInitTextures(const FSprite* Sprite)
{
    if(isConstSprite(Sprite) && *(Sprite->u.textureIndirect) == NULL) {
        *(Sprite->u.textureIndirect) =
            f_platform_api__textureNew(&Sprite->pixels);
    }
}
#endif

static FSprite* spriteNew(const FPixels* Pixels, int X, int Y, int FrameWidth, int FrameHeight)
{
    FVecInt gridDim;

    if((FrameWidth < 1 || FrameHeight < 1) && FrameWidth != FrameHeight) {
        F__FATAL("FrameWidth = %d, FrameHeight = %d", FrameWidth, FrameHeight);
    }

    // If start is at origin and frame size is negative, use entire image
    if(X == 0 && Y == 0 && FrameWidth < 0) {
        gridDim = Pixels->size;
    } else {
        gridDim = f_pixels__boundsFind(Pixels, X, Y);
    }

    // If frame size is not positive, make a single frame the entire area size
    if(FrameWidth < 1) {
        FrameWidth = gridDim.x;
        FrameHeight = gridDim.y;
    }

    unsigned framesNum =
        (unsigned)((gridDim.x / FrameWidth) * (gridDim.y / FrameHeight));

    if(framesNum == 0) {
        F__FATAL("Cannot create a %dx%d sprite from %dx%d @ %d,%d",
                 FrameWidth,
                 FrameHeight,
                 Pixels->size.x,
                 Pixels->size.y,
                 X,
                 Y);
    }

    FSprite* s = f_pool__alloc(F_POOL__SPRITE);

    f_pixels__init(
        &s->pixels, FrameWidth, FrameHeight, framesNum, F_PIXELS__ALLOC);

    unsigned f = 0;
    int endX = X + gridDim.x - (gridDim.x % FrameWidth);
    int endY = Y + gridDim.y - (gridDim.y % FrameHeight);

    for(int y = Y; y < endY; y += FrameHeight) {
        for(int x = X; x < endX; x += FrameWidth, f++) {
            f_pixels__copyFrameEx(&s->pixels, f, Pixels, x, y);
        }
    }

    s->u.texture = f_platform_api__textureNew(&s->pixels);

    return s;
}

FSprite* f_sprite_newFromImage(const char* Path, int X, int Y, int FrameWidth, int FrameHeight)
{
    FPixels* pixels = f_platform_api__imageRead(Path);

    if(pixels == NULL) {
        F__FATAL("f_sprite_newFromImage(%s): Cannot open file", Path);
    }

    if(FrameWidth < 1 || FrameHeight < 1) {
        char* suffix = f_str_suffixGetFromLast(Path, '_');

        if(suffix) {
            int n = sscanf(suffix, "%dx%d", &FrameWidth, &FrameHeight);

            if(n != 2) {
                FrameWidth = 0;
                FrameHeight = 0;
            }

            f_mem_free(suffix);
        }
    }

    FSprite* s = spriteNew(pixels, X, Y, FrameWidth, FrameHeight);

    f_pixels__free(pixels);

    return s;
}

FSprite* f_sprite_newFromSprite(const FSprite* Sheet, int X, int Y, int FrameWidth, int FrameHeight)
{
    return spriteNew(&Sheet->pixels, X, Y, FrameWidth, FrameHeight);
}

FSprite* f_sprite_newBlank(int Width, int Height, unsigned Frames, bool ColorKeyed)
{
    if(Frames == 0) {
        F__FATAL("f_sprite_newBlank: 0 frames");
    }

    FSprite* s = f_pool__alloc(F_POOL__SPRITE);

    f_pixels__init(&s->pixels, Width, Height, Frames, F_PIXELS__ALLOC);

    for(unsigned f = Frames; f--; ) {
        if(ColorKeyed) {
            f_pixels__fill(&s->pixels, f, f_color__key);
        }
    }

    s->u.texture = f_platform_api__textureNew(&s->pixels);

    return s;
}

FSprite* f_sprite_dup(const FSprite* Sprite)
{
    #if F_CONFIG_SCREEN_RENDER != F_SCREEN_RENDER_SOFTWARE
        lazyInitTextures(Sprite);
    #endif

    FSprite* s = f_pool__alloc(F_POOL__SPRITE);

    f_pixels__copy(&s->pixels, &Sprite->pixels);
    s->u.texture = f_platform_api__textureDup(
                    getSpriteTexture(Sprite), &Sprite->pixels);

    return s;
}

void f_sprite_free(FSprite* Sprite)
{
    if(Sprite == NULL) {
        return;
    }

    f_platform_api__textureFree(Sprite->u.texture);
    f_pixels__free(&Sprite->pixels);

    f_pool_release(Sprite);
}

void f_sprite_blit(const FSprite* Sprite, unsigned Frame, int X, int Y)
{
    #if F_CONFIG_SCREEN_RENDER != F_SCREEN_RENDER_SOFTWARE
        lazyInitTextures(Sprite);
    #endif

    Frame %= Sprite->pixels.framesNum;

    FVecInt spriteSize = Sprite->pixels.size;

    if(f__align.x == F_ALIGN_X_CENTER) {
        X -= spriteSize.x >> 1;
    } else if(f__align.x == F_ALIGN_X_RIGHT) {
        X -= spriteSize.x;
    }

    if(f__align.y == F_ALIGN_Y_CENTER) {
        Y -= spriteSize.y >> 1;
    } else if(f__align.y == F_ALIGN_Y_BOTTOM) {
        Y -= spriteSize.y;
    }

    f_platform_api__textureBlit(
        getSpriteTexture(Sprite), &Sprite->pixels, Frame, X, Y);
}

void f_sprite_blitEx(const FSprite* Sprite, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    #if F_CONFIG_SCREEN_RENDER != F_SCREEN_RENDER_SOFTWARE
        lazyInitTextures(Sprite);
    #endif

    Frame %= Sprite->pixels.framesNum;

    CenterX = f_math_clamp(CenterX, -F_FIX_ONE, F_FIX_ONE);
    CenterY = f_math_clamp(CenterY, -F_FIX_ONE, F_FIX_ONE);

    f_platform_api__textureBlitEx(getSpriteTexture(Sprite),
                                  &Sprite->pixels,
                                  Frame,
                                  X,
                                  Y,
                                  Scale,
                                  f_fix_angleWrap(Angle),
                                  CenterX,
                                  CenterY);
}

void f_sprite_swapColor(FSprite* Sprite, FColorPixel OldColor, FColorPixel NewColor)
{
    for(unsigned f = Sprite->pixels.framesNum; f--; ) {
        FColorPixel* buffer = f_pixels__bufferGetStart(&Sprite->pixels, f);

        for(unsigned i = Sprite->pixels.bufferLen; i--; ) {
            if(buffer[i] == OldColor) {
                buffer[i] = NewColor;
            }
        }
    }

    f_platform_api__textureFree(Sprite->u.texture);
    Sprite->u.texture = f_platform_api__textureNew(&Sprite->pixels);
}

void f_sprite_swapColors(FSprite* Sprite, const FColorPixel* OldColors, const FColorPixel* NewColors, unsigned NumColors)
{
    for(unsigned f = Sprite->pixels.framesNum; f--; ) {
        FColorPixel* buffer = f_pixels__bufferGetStart(&Sprite->pixels, f);

        for(unsigned i = Sprite->pixels.bufferLen; i--; ) {
            const FColorPixel pixel = buffer[i];

            for(unsigned c = NumColors; c--; ) {
                if(pixel == OldColors[c]) {
                    buffer[i] = NewColors[c];
                    break;
                }
            }
        }
    }

    f_platform_api__textureFree(Sprite->u.texture);
    Sprite->u.texture = f_platform_api__textureNew(&Sprite->pixels);
}

FVecInt f_sprite_sizeGet(const FSprite* Sprite)
{
    return Sprite->pixels.size;
}

int f_sprite_sizeGetWidth(const FSprite* Sprite)
{
    return Sprite->pixels.size.x;
}

int f_sprite_sizeGetHeight(const FSprite* Sprite)
{
    return Sprite->pixels.size.y;
}

unsigned f_sprite_framesNumGet(const FSprite* Sprite)
{
    return Sprite->pixels.framesNum;
}

const FColorPixel* f_sprite_pixelsGetBuffer(const FSprite* Sprite, unsigned Frame)
{
    return f_pixels__bufferGetStartConst(&Sprite->pixels, Frame);
}

FColorPixel f_sprite_pixelsGetValue(const FSprite* Sprite, unsigned Frame, int X, int Y)
{
    return f_pixels__bufferGetValue(&Sprite->pixels, Frame, X, Y);
}
