/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#if !F_CONFIG_RENDER_SOFTWARE
static inline void lazyInitTextures(FSprite* Sprite)
{
    if(Sprite->texture == NULL) {
        Sprite->texture = f_platform_api__textureNew(&Sprite->pixels);
    }
}
#endif

static FSprite* spriteNew(const FPixels* Pixels, int X, int Y, int FrameWidth, int FrameHeight)
{
    FVecInt gridDim;

    #if F_CONFIG_DEBUG
        if((FrameWidth < 1 || FrameHeight < 1) && FrameWidth != FrameHeight) {
            F__FATAL(
                "FrameWidth = %d, FrameHeight = %d", FrameWidth, FrameHeight);
        }
    #endif

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

    FSprite* s = f_mem_malloc(sizeof(FSprite));

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

    s->texture = f_platform_api__textureNew(&s->pixels);

    return s;
}

#if F_CONFIG_LIB_PNG
FSprite* f_sprite_newFromPng(const char* Path, int X, int Y, int FrameWidth, int FrameHeight)
{
    FPixels* pixels = f_png__read(Path);

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
#endif

FSprite* f_sprite_newFromSprite(const FSprite* Sheet, int X, int Y, int FrameWidth, int FrameHeight)
{
    return spriteNew(&Sheet->pixels, X, Y, FrameWidth, FrameHeight);
}

FSprite* f_sprite_newBlank(int Width, int Height, unsigned Frames, bool ColorKeyed)
{
    if(Frames == 0) {
        F__FATAL("f_sprite_newBlank: Frames == 0");
    }

    FSprite* s = f_mem_malloc(sizeof(FSprite));

    f_pixels__init(&s->pixels, Width, Height, Frames, F_PIXELS__ALLOC);

    for(unsigned f = Frames; f--; ) {
        if(ColorKeyed) {
            f_pixels__fill(&s->pixels, f, f_color__key);
        }
    }

    s->texture = f_platform_api__textureNew(&s->pixels);

    return s;
}

FSprite* f_sprite_dup(const FSprite* Sprite)
{
    FSprite* s = f_mem_malloc(sizeof(FSprite));

    #if !F_CONFIG_RENDER_SOFTWARE
        lazyInitTextures((FSprite*)Sprite);
    #endif

    f_pixels__copy(&s->pixels, &Sprite->pixels);

    s->texture = f_platform_api__textureDup(Sprite->texture, &Sprite->pixels);

    return s;
}

void f_sprite_free(FSprite* Sprite)
{
    if(Sprite == NULL) {
        return;
    }

    if(F_FLAGS_TEST_ANY(Sprite->pixels.flags, F_PIXELS__CONST)) {
        #if !F_CONFIG_RENDER_SOFTWARE
            f_platform_api__textureFree(Sprite->texture);

            // Sprite may be re-used later
            Sprite->texture = NULL;
        #endif

        return;
    }

    f_platform_api__textureFree(Sprite->texture);
    f_pixels__free(&Sprite->pixels);

    f_mem_free(Sprite);
}

void f_sprite_blit(const FSprite* Sprite, unsigned Frame, int X, int Y)
{
    #if !F_CONFIG_RENDER_SOFTWARE
        lazyInitTextures((FSprite*)Sprite);
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

    f_platform_api__textureBlit(Sprite->texture, &Sprite->pixels, Frame, X, Y);
}

void f_sprite_blitEx(const FSprite* Sprite, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    #if !F_CONFIG_RENDER_SOFTWARE
        lazyInitTextures((FSprite*)Sprite);
    #endif

    Frame %= Sprite->pixels.framesNum;

    CenterX = f_math_clamp(CenterX, -F_FIX_ONE, F_FIX_ONE);
    CenterY = f_math_clamp(CenterY, -F_FIX_ONE, F_FIX_ONE);

    f_platform_api__textureBlitEx(Sprite->texture,
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
    #if F_CONFIG_DEBUG
        if(F_FLAGS_TEST_ANY(Sprite->pixels.flags, F_PIXELS__CONST)) {
            F__FATAL("f_sprite_swapColor: Const sprite");
        }
    #endif

    for(unsigned f = Sprite->pixels.framesNum; f--; ) {
        FColorPixel* buffer = f_pixels__bufferGetStart(&Sprite->pixels, f);

        for(unsigned i = Sprite->pixels.bufferLen; i--; ) {
            if(buffer[i] == OldColor) {
                buffer[i] = NewColor;
            }
        }
    }

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__textureFree(Sprite->texture);

        Sprite->texture = f_platform_api__textureNew(&Sprite->pixels);
    #endif
}

void f_sprite_swapColors(FSprite* Sprite, const FColorPixel* OldColors, const FColorPixel* NewColors, unsigned NumColors)
{
    #if F_CONFIG_DEBUG
        if(F_FLAGS_TEST_ANY(Sprite->pixels.flags, F_PIXELS__CONST)) {
            F__FATAL("f_sprite_swapColors: Const sprite");
        }
    #endif

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

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__textureFree(Sprite->texture);

        Sprite->texture = f_platform_api__textureNew(&Sprite->pixels);
    #endif
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
    return f_pixels__bufferGetStart(&Sprite->pixels, Frame);
}

FColorPixel f_sprite_pixelsGetValue(const FSprite* Sprite, unsigned Frame, int X, int Y)
{
    return f_pixels__bufferGetValue(&Sprite->pixels, Frame, X, Y);
}

#if F_CONFIG_RENDER_SOFTWARE
void f_sprite__textureUpdate(FSprite* Sprite, unsigned Frame)
{
    f_platform_api__textureUpdate(Sprite->texture, &Sprite->pixels, Frame);
}
#else
FPlatformTextureScreen* f_sprite__textureGet(const FSprite* Sprite)
{
    return f_platform_api__textureSpriteToScreen(Sprite->texture);
}
#endif
