/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "f_sdl_blit.v.h"
#include <faur.v.h>

#if F_CONFIG_RENDER_SDL2
#include <SDL2/SDL.h>

typedef enum {
    F_SIDE__INVALID = -1,
    F_SIDE__NORMAL, // non-colorkey: alpha:0xff
    F_SIDE__COLORMOD_BITMAP, // colorkey: RGB:0xffffff
    F_SIDE__COLORMOD_FLAT, // non-colorkey: RGB:0xffffff
    F_SIDE__ALPHA_MASK, // all: alpha:RGB
    F_SIDE__NUM
} FTextureSide;

typedef struct {
    SDL_Texture* sides[F_SIDE__NUM];
} FTexture;

extern SDL_Renderer* f__sdlRenderer;

FPlatformTextureScreen* f_platform_api__textureSpriteToScreen(FPlatformTexture* SpriteTexture)
{
    return ((FTexture*)SpriteTexture)->sides[F_SIDE__NORMAL];
}

FPlatformTexture* f_platform_api__textureNew(const FPixels* Pixels)
{
    FTexture* texture = f_mem_mallocz(sizeof(FTexture));

    unsigned totalBufferLen = Pixels->bufferLen * Pixels->framesNum;
    unsigned totalBufferSize = Pixels->bufferSize * Pixels->framesNum;

    const FColorPixel* original = f_pixels__bufferGetStart(Pixels, 0);
    FColorPixel* buffer = f_mem_dup(original, totalBufferSize);

    for(int s = 0; s < F_SIDE__NUM; s++) {
        switch(s) {
            case F_SIDE__NORMAL: {
                for(unsigned i = totalBufferLen; i--; ) {
                    if(original[i] != f_color__key) {
                        // Set full alpha for non-transparent pixel
                        buffer[i] |= (FColorPixel)
                            ((1 << F__PX_BITS_A) - 1) << F__PX_SHIFT_A;
                    }
                }
            } break;

            case F_SIDE__COLORMOD_BITMAP: {
                for(unsigned i = totalBufferLen; i--; ) {
                    if(original[i] == f_color__key) {
                        // Set full color for transparent pixel
                        buffer[i] |= f_color_pixelFromHex(0xffffff);
                    }
                }
            } break;

            case F_SIDE__COLORMOD_FLAT: {
                for(unsigned i = totalBufferLen; i--; ) {
                    if(original[i] != f_color__key) {
                        // Set full color for non-transparent pixel
                        buffer[i] |= f_color_pixelFromHex(0xffffff);
                    }
                }
            } break;

            case F_SIDE__ALPHA_MASK: {
                for(unsigned i = totalBufferLen; i--; ) {
                    int alpha = f_color_pixelToRgbAny(original[i]);

                    buffer[i] =
                        original[i] | (FColorPixel)(alpha << F__PX_SHIFT_A);
                }
            } break;
        }

        SDL_Texture* tex = SDL_CreateTexture(
                            f__sdlRenderer,
                            F_SDL__PIXEL_FORMAT,
                            SDL_TEXTUREACCESS_TARGET,
                            Pixels->size.x,
                            Pixels->size.y * (int)Pixels->framesNum);

        if(tex == NULL) {
            F__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        if(SDL_UpdateTexture(
            tex, NULL, buffer, Pixels->size.x * (int)sizeof(FColorPixel)) < 0) {

            F__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
        }

        texture->sides[s] = tex;
    }

    f_mem_free(buffer);

    return texture;
}

FPlatformTexture* f_platform_api__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels)
{
    const FTexture* texSrc = Texture;
    FTexture* texDst = f_mem_mallocz(sizeof(FTexture));

    if(SDL_RenderSetClipRect(f__sdlRenderer, NULL) < 0) {
        f_out__error("SDL_RenderSetClipRect: %s", SDL_GetError());
    }

    for(int s = 0; s < F_SIDE__NUM; s++) {
        SDL_Texture* tex = SDL_CreateTexture(
                            f__sdlRenderer,
                            F_SDL__PIXEL_FORMAT,
                            SDL_TEXTUREACCESS_TARGET,
                            Pixels->size.x,
                            Pixels->size.y * (int)Pixels->framesNum);

        if(tex == NULL) {
            F__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        if(SDL_SetRenderTarget(f__sdlRenderer, tex) < 0) {
            F__FATAL("SDL_SetRenderTarget: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(texSrc->sides[s], SDL_BLENDMODE_NONE) < 0) {
            f_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
        }

        if(SDL_RenderCopy(f__sdlRenderer, texSrc->sides[s], NULL, NULL) < 0) {
            F__FATAL("SDL_RenderCopy: %s", SDL_GetError());
        }

        texDst->sides[s] = tex;
    }

    // Restore user settings

    if(SDL_SetRenderTarget(f__sdlRenderer, f__screen.texture) < 0) {
        F__FATAL("SDL_SetRenderTarget: %s", SDL_GetError());
    }

    f_platform_api__screenClipSet();

    return texDst;
}

void f_platform_api__textureFree(FPlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    FTexture* texture = Texture;

    for(int s = F_SIDE__NUM; s--; ) {
        if(texture->sides[s]) {
            SDL_DestroyTexture(texture->sides[s]);
        }
    }

    f_mem_free(texture);
}

void f_platform_api__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    f_platform_api__textureBlitEx(Texture,
                                  Pixels,
                                  Frame,
                                  X + Pixels->size.x / 2,
                                  Y + Pixels->size.y / 2,
                                  F_FIX_ONE,
                                  0,
                                  0,
                                  0);
}

void f_platform_api__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    SDL_Texture* tex;
    SDL_BlendMode blend =
        (SDL_BlendMode)f_platform_sdl_video__pixelBlendToSdlBlend();

    const FTexture* texture = Texture;

    if(f__color.fillBlit) {
        tex = texture->sides[F_SIDE__COLORMOD_FLAT];
    } else if(blend == SDL_BLENDMODE_MOD) {
        tex = texture->sides[F_SIDE__COLORMOD_BITMAP];
    } else if(f__color.blend == F_COLOR_BLEND_ALPHA_MASK) {
        tex = texture->sides[F_SIDE__ALPHA_MASK];
    } else {
        tex = texture->sides[F_SIDE__NORMAL];
    }

    if(SDL_SetTextureBlendMode(tex, blend) < 0) {
        f_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
    }

    if(SDL_SetTextureAlphaMod(
        tex, f_platform_sdl_video__pixelAlphaToSdlAlpha()) < 0) {

        f_out__error("SDL_SetTextureAlphaMod: %s", SDL_GetError());
    }

    bool mod = f__color.fillBlit || f__color.blend == F_COLOR_BLEND_ALPHA_MASK;

    if(mod && SDL_SetTextureColorMod(tex,
                                     (uint8_t)f__color.rgb.r,
                                     (uint8_t)f__color.rgb.g,
                                     (uint8_t)f__color.rgb.b) < 0) {

        f_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
    }

    Y += f__screen.yOffset;

    FVecInt halfSize = {Pixels->size.x / 2, Pixels->size.y / 2};

    SDL_Point center = {
        f_fix_toInt(
            f_fix_mul(f_fix_fromInt(halfSize.x) + CenterX * halfSize.x, Scale)),
        f_fix_toInt(
            f_fix_mul(f_fix_fromInt(halfSize.y) + CenterY * halfSize.y, Scale))
    };

    SDL_Rect src = {0,
                    Pixels->size.y * (int)Frame,
                    Pixels->size.x,
                    Pixels->size.y};

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     f_fix_toInt(Pixels->size.x * Scale),
                     f_fix_toInt(Pixels->size.y * Scale)};

    if(SDL_RenderCopyEx(f__sdlRenderer,
                        tex,
                        &src,
                        &dest,
                        360 - 360 * Angle / F_FIX_ANGLES_NUM,
                        &center,
                        SDL_FLIP_NONE) < 0) {

        f_out__error("SDL_RenderCopyEx: %s", SDL_GetError());
    }

    if(mod && SDL_SetTextureColorMod(tex, 0xff, 0xff, 0xff) < 0) {
        f_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
    }
}
#endif // F_CONFIG_RENDER_SDL2
