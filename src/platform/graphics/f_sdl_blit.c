/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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
    F_TEXTURE__INVALID = -1,
    F_TEXTURE__NORMAL, // non-colorkey: alpha:0xff
    F_TEXTURE__COLORMOD_BITMAP, // colorkey: RGB:0xffffff
    F_TEXTURE__COLORMOD_FLAT, // non-colorkey: RGB:0xffffff
    F_TEXTURE__ALPHA_MASK, // all: alpha:RGB
    F_TEXTURE__NUM
} FPlatformTextureVersion;

struct FPlatformTexture {
    SDL_Texture* texture[F_TEXTURE__NUM];
};

extern SDL_Renderer* f__sdlRenderer;

FPlatformTextureScreen* f_platform_api__textureSpriteToScreen(FPlatformTexture* SpriteTexture)
{
    return SpriteTexture->texture[F_TEXTURE__NORMAL];
}

FPlatformTexture* f_platform_api__textureNew(const FPixels* Pixels, unsigned Frame)
{
    FPlatformTexture* texture = f_mem_zalloc(sizeof(FPlatformTexture));
    const FColorPixel* original = f_pixels__bufferGetStart(Pixels, Frame);
    FColorPixel* buffer = f_mem_dup(original, Pixels->bufferSize);

    for(int t = 0; t < F_TEXTURE__NUM; t++) {
        switch(t) {
            case F_TEXTURE__NORMAL: {
                for(int i = Pixels->size.x * Pixels->size.y; i--; ) {
                    if(original[i] != f_color__key) {
                        // Set full alpha for non-transparent pixel
                        buffer[i] |= (FColorPixel)
                            ((1 << F__PX_BITS_A) - 1) << F__PX_SHIFT_A;
                    }
                }
            } break;

            case F_TEXTURE__COLORMOD_BITMAP: {
                for(int i = Pixels->size.x * Pixels->size.y; i--; ) {
                    if(original[i] == f_color__key) {
                        // Set full color for transparent pixel
                        buffer[i] |= f_color_pixelFromHex(0xffffff);
                    }
                }
            } break;

            case F_TEXTURE__COLORMOD_FLAT: {
                for(int i = Pixels->size.x * Pixels->size.y; i--;) {
                    if(original[i] != f_color__key) {
                        // Set full color for non-transparent pixel
                        buffer[i] |= f_color_pixelFromHex(0xffffff);
                    }
                }
            } break;

            case F_TEXTURE__ALPHA_MASK: {
                for(int i = Pixels->size.x * Pixels->size.y; i--;) {
                    int alpha = f_color_pixelToRgbAny(original[i]);

                    buffer[i] =
                        original[i] | (FColorPixel)(alpha << F__PX_SHIFT_A);
                }
            } break;
        }

        SDL_Texture* tex = SDL_CreateTexture(f__sdlRenderer,
                                             F_SDL__PIXEL_FORMAT,
                                             SDL_TEXTUREACCESS_TARGET,
                                             Pixels->size.x,
                                             Pixels->size.y);
        if(tex == NULL) {
            F__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        int ret = SDL_UpdateTexture(tex,
                                    NULL,
                                    buffer,
                                    Pixels->size.x * (int)sizeof(FColorPixel));

        if(ret < 0) {
            F__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND) < 0) {
            f_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
        }

        texture->texture[t] = tex;
    }

    f_mem_free(buffer);

    return texture;
}

void f_platform_api__textureFree(FPlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    for(int t = F_TEXTURE__NUM; t--; ) {
        if(Texture->texture[t]) {
            SDL_DestroyTexture(Texture->texture[t]);
        }
    }

    f_mem_free(Texture);
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
    F_UNUSED(Frame);

    SDL_Texture* tex;
    SDL_BlendMode blend =
        (SDL_BlendMode)f_platform_sdl_video__pixelBlendToSdlBlend();

    bool mod = f__color.fillBlit || f__color.blend == F_COLOR_BLEND_ALPHA_MASK;

    if(f__color.fillBlit) {
        tex = Texture->texture[F_TEXTURE__COLORMOD_FLAT];
    } else if(blend == SDL_BLENDMODE_MOD) {
        tex = Texture->texture[F_TEXTURE__COLORMOD_BITMAP];
    } else if(f__color.blend == F_COLOR_BLEND_ALPHA_MASK) {
        tex = Texture->texture[F_TEXTURE__ALPHA_MASK];
    } else {
        tex = Texture->texture[F_TEXTURE__NORMAL];
    }

    if(SDL_SetTextureBlendMode(tex, blend) < 0) {
        f_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
    }

    if(SDL_SetTextureAlphaMod(
        tex, f_platform_sdl_video__pixelAlphaToSdlAlpha()) < 0) {

        f_out__error("SDL_SetTextureAlphaMod: %s", SDL_GetError());
    }

    if(mod) {
        if(SDL_SetTextureColorMod(tex,
                                  (uint8_t)f__color.rgb.r,
                                  (uint8_t)f__color.rgb.g,
                                  (uint8_t)f__color.rgb.b) < 0) {

            f_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }

    FVectorInt halfSize = {Pixels->size.x / 2, Pixels->size.y / 2};

    SDL_Point center = {
        f_fix_toInt(
            f_fix_mul(f_fix_fromInt(halfSize.x) + CenterX * halfSize.x, Scale)),
        f_fix_toInt(
            f_fix_mul(f_fix_fromInt(halfSize.y) + CenterY * halfSize.y, Scale))
    };

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     f_fix_toInt(Pixels->size.x * Scale),
                     f_fix_toInt(Pixels->size.y * Scale)};

    if(SDL_RenderCopyEx(f__sdlRenderer,
                        tex,
                        NULL,
                        &dest,
                        360 - 360 * Angle / F_FIX_ANGLES_NUM,
                        &center,
                        SDL_FLIP_NONE) < 0) {

        f_out__error("SDL_RenderCopyEx: %s", SDL_GetError());
    }

    if(mod) {
        if(SDL_SetTextureColorMod(tex, 0xff, 0xff, 0xff) < 0) {
            f_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }
}
#endif // F_CONFIG_RENDER_SDL2
