/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "a_sdl_blit.v.h"
#include <a2x.v.h>

#if A_CONFIG_LIB_RENDER_SDL
#include <SDL2/SDL.h>

typedef enum {
    A_TEXTURE__INVALID = -1,
    A_TEXTURE__NORMAL, // non-colorkey: alpha:0xff
    A_TEXTURE__COLORMOD_BITMAP, // colorkey: RGB:0xffffff
    A_TEXTURE__COLORMOD_FLAT, // non-colorkey: RGB:0xffffff
    A_TEXTURE__NUM
} APlatformTextureVersion;

struct APlatformTexture {
    SDL_Texture* texture[A_TEXTURE__NUM];
};

extern SDL_Renderer* a__sdlRenderer;

APlatformTexture* a_platform_api__textureNew(const APixels* Pixels, unsigned Frame)
{
    APlatformTexture* texture = a_mem_zalloc(sizeof(APlatformTexture));
    const APixel* original = a_pixels__bufferGetStart(Pixels, Frame);
    APixel* buffer = a_mem_dup(original, Pixels->bufferSize);

    for(int t = 0; t < A_TEXTURE__NUM; t++) {
        switch(t) {
            case A_TEXTURE__NORMAL: {
                for(int i = Pixels->w * Pixels->h; i--; ) {
                    if(original[i] != a_color__key) {
                        // Set full alpha for non-transparent pixel
                        buffer[i] |= (APixel)A__PX_MASK_A << A__PX_SHIFT_A;
                    }
                }
            } break;

            case A_TEXTURE__COLORMOD_BITMAP: {
                for(int i = Pixels->w * Pixels->h; i--; ) {
                    if(original[i] == a_color__key) {
                        // Set full color for transparent pixel
                        buffer[i] |= a_pixel_fromHex(0xffffff);
                    }
                }
            } break;

            case A_TEXTURE__COLORMOD_FLAT: {
                for(int i = Pixels->w * Pixels->h; i--;) {
                    if(original[i] != a_color__key) {
                        // Set full color for non-transparent pixel
                        buffer[i] |= a_pixel_fromHex(0xffffff);
                    }
                }
            } break;
        }

        SDL_Texture* tex = SDL_CreateTexture(a__sdlRenderer,
                                             A_SDL__PIXEL_FORMAT,
                                             SDL_TEXTUREACCESS_TARGET,
                                             Pixels->w,
                                             Pixels->h);
        if(tex == NULL) {
            A__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        if(SDL_UpdateTexture(
            tex, NULL, buffer, Pixels->w * (int)sizeof(APixel)) < 0) {

            A__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND) < 0) {
            a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
        }

        texture->texture[t] = tex;
    }

    a_mem_free(buffer);

    return texture;
}

void a_platform_api__textureFree(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    for(int t = A_TEXTURE__NUM; t--; ) {
        if(Texture->texture[t]) {
            SDL_DestroyTexture(Texture->texture[t]);
        }
    }

    a_mem_free(Texture);
}

void a_platform_api__textureBlit(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    a_platform_api__textureBlitEx(Texture,
                                  Pixels,
                                  Frame,
                                  X + Pixels->w / 2,
                                  Y + Pixels->h / 2,
                                  A_FIX_ONE,
                                  0,
                                  0,
                                  0);
}

void a_platform_api__textureBlitEx(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, AFix CenterX, AFix CenterY)
{
    A_UNUSED(Frame);

    SDL_Texture* tex;
    SDL_BlendMode blend =
        (SDL_BlendMode)a_platform_sdl_video__pixelBlendToSdlBlend();

    if(a__color.fillBlit) {
        tex = Texture->texture[A_TEXTURE__COLORMOD_FLAT];
    } else if(blend == SDL_BLENDMODE_MOD) {
        tex = Texture->texture[A_TEXTURE__COLORMOD_BITMAP];
    } else {
        tex = Texture->texture[A_TEXTURE__NORMAL];
    }

    if(SDL_SetTextureBlendMode(tex, blend) < 0) {
        a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
    }

    if(SDL_SetTextureAlphaMod(
        tex, a_platform_sdl_video__pixelAlphaToSdlAlpha()) < 0) {

        a_out__error("SDL_SetTextureAlphaMod: %s", SDL_GetError());
    }

    if(a__color.fillBlit) {
        if(SDL_SetTextureColorMod(tex,
                                  (uint8_t)a__color.rgb.r,
                                  (uint8_t)a__color.rgb.g,
                                  (uint8_t)a__color.rgb.b) < 0) {

            a_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }

    AVectorInt halfSize = {Pixels->w / 2, Pixels->h / 2};

    SDL_Point center = {
        a_fix_toInt(
            a_fix_mul(a_fix_fromInt(halfSize.x) + CenterX * halfSize.x, Scale)),
        a_fix_toInt(
            a_fix_mul(a_fix_fromInt(halfSize.y) + CenterY * halfSize.y, Scale))
    };

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     a_fix_toInt(Pixels->w * Scale),
                     a_fix_toInt(Pixels->h * Scale)};

    if(SDL_RenderCopyEx(a__sdlRenderer,
                        tex,
                        NULL,
                        &dest,
                        360 - 360 * Angle / A_FIX_ANGLES_NUM,
                        &center,
                        SDL_FLIP_NONE) < 0) {

        a_out__error("SDL_RenderCopyEx: %s", SDL_GetError());
    }

    if(a__color.fillBlit) {
        if(SDL_SetTextureColorMod(tex, 0xff, 0xff, 0xff) < 0) {
            a_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }
}
#endif // A_CONFIG_LIB_RENDER_SDL
