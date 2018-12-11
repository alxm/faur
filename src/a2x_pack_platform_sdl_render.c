/*
    Copyright 2016-2018 Alex Margarit

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

#include "a2x_pack_platform_sdl_render.v.h"

#if A_BUILD_RENDER_SDL
#include <SDL.h>

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_platform_sdl_video.v.h"

#define A__NUM_SPRITE_TEXTURES 2

struct APlatformTexture {
    APixel* pixels;
    size_t pixelsSize;
    int w, h;
    SDL_Texture* texture[A__NUM_SPRITE_TEXTURES];
};

static inline SDL_BlendMode pixelBlendToSdlBlend(void)
{
    switch(a_pixel__state.blend) {
        case A_PIXEL_BLEND_MOD:
            return SDL_BLENDMODE_MOD;

        case A_PIXEL_BLEND_ADD:
            return SDL_BLENDMODE_ADD;

        default:
            return SDL_BLENDMODE_BLEND;
    }
}

static inline uint8_t pixelAlphaToSdlAlpha(void)
{
    switch(a_pixel__state.blend) {
        case A_PIXEL_BLEND_RGBA:
        case A_PIXEL_BLEND_RGB25:
        case A_PIXEL_BLEND_RGB50:
        case A_PIXEL_BLEND_RGB75:
            return (uint8_t)a_pixel__state.alpha;

        default:
            return SDL_ALPHA_OPAQUE;
    }
}

void a_platform__renderSetDrawColor(void)
{
    if(SDL_SetRenderDrawColor(a__sdlRenderer,
                              (uint8_t)a_pixel__state.red,
                              (uint8_t)a_pixel__state.green,
                              (uint8_t)a_pixel__state.blue,
                              pixelAlphaToSdlAlpha()) < 0) {

        a_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
    }
}

void a_platform__renderSetBlendMode(void)
{
    if(SDL_SetRenderDrawBlendMode(a__sdlRenderer, pixelBlendToSdlBlend()) < 0) {
        a_out__error("SDL_SetRenderDrawBlendMode: %s", SDL_GetError());
    }
}

void a_platform__drawPixel(int X, int Y)
{
    if(SDL_RenderDrawPoint(a__sdlRenderer, X, Y) < 0) {
        a_out__error("SDL_RenderDrawPoint: %s", SDL_GetError());
    }
}

void a_platform__drawLine(int X1, int Y1, int X2, int Y2)
{
    if(SDL_RenderDrawLine(a__sdlRenderer, X1, Y1, X2, Y2) < 0) {
        a_out__error("SDL_RenderDrawLine: %s", SDL_GetError());
    }
}

void a_platform__drawHLine(int X1, int X2, int Y)
{
    a_platform__drawRectangleFilled(X1, Y, X2 - X1 + 1, 1);
}

void a_platform__drawVLine(int X, int Y1, int Y2)
{
    a_platform__drawRectangleFilled(X, Y1, 1, Y2 - Y1 + 1);
}

void a_platform__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderFillRect(a__sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderFillRect: %s", SDL_GetError());
    }
}

void a_platform__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    a_platform__drawRectangleFilled(X, Y, Width, 1);

    if(Height <= 1) {
        return;
    }

    a_platform__drawRectangleFilled(X, Y + Height - 1, Width, 1);

    if(Width <= 1 || Height <= 2) {
        return;
    }

    a_platform__drawRectangleFilled(X, Y + 1, 1, Height - 2);
    a_platform__drawRectangleFilled(X + Width - 1, Y + 1, 1, Height - 2);
}

void a_platform__drawCircleOutline(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            a_platform__drawRectangleFilled(X - 1, Y - 1, 2, 2);
        }

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    int numPointPairs = 0;

    while(x > y) {
        numPointPairs++;

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;
        }
    }

    if(x == y) {
        numPointPairs++;
    }

    SDL_Point scanlines[numPointPairs * 4][2];

    x = Radius;
    y = 0;
    error = -Radius / 2;

    int scanline1 = 0;
    int x1 = X - 1 - x;
    int x2 = X + x;
    int y1 = Y - 1 - y;

    int scanline2 = numPointPairs;
    int y2 = Y + y;

    int scanline3 = numPointPairs * 2;
    int x3 = X - 1 - y;
    int x4 = X + y;
    int y3 = Y - 1 - x;

    int scanline4 = numPointPairs * 3;
    int y4 = Y + x;

    while(x > y) {
        scanlines[scanline1][0] = (SDL_Point){x1, y1};
        scanlines[scanline1][1] = (SDL_Point){x2, y1};

        scanlines[scanline2][0] = (SDL_Point){x1, y2};
        scanlines[scanline2][1] = (SDL_Point){x2, y2};

        scanlines[scanline3][0] = (SDL_Point){x3, y3};
        scanlines[scanline3][1] = (SDL_Point){x4, y3};

        scanlines[scanline4][0] = (SDL_Point){x3, y4};
        scanlines[scanline4][1] = (SDL_Point){x4, y4};

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        scanline1++;
        scanline2++;
        scanline3++;
        scanline4++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        scanlines[scanline3][0] = (SDL_Point){x3, y3};
        scanlines[scanline3][1] = (SDL_Point){x4, y3};

        scanlines[scanline4][0] = (SDL_Point){x3, y4};
        scanlines[scanline4][1] = (SDL_Point){x4, y4};
    }

    if(SDL_RenderDrawPoints(
        a__sdlRenderer, (SDL_Point*)scanlines, numPointPairs * 4 * 2) < 0) {

        a_out__error("SDL_RenderDrawPoints: %s", SDL_GetError());
    }
}

void a_platform__drawCircleFilled(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            a_platform__drawRectangleFilled(X - 1, Y - 1, 2, 2);
        }

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    const int numScanlines = (Radius + 1) * 2;
    SDL_Rect scanlines[numScanlines];

    int scanline1 = numScanlines / 2 - 1 - y;
    int x1 = X - 1 - x;
    int y1 = Y - 1 - y;
    int w1 = 2 * x + 2;

    int scanline2 = numScanlines / 2 + y;
    int y2 = Y + y;

    int scanline3 = numScanlines / 2 - 1 - x;
    int x3 = X - 1 - y;
    int y3 = Y - 1 - x;
    int w3 = 2 * y + 2;

    int scanline4 = numScanlines / 2 + x;
    int y4 = Y + x;

    while(x > y) {
        scanlines[scanline1] = (SDL_Rect){x1, y1, w1, 1};
        scanlines[scanline2] = (SDL_Rect){x1, y2, w1, 1};

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        scanline1--;
        y1--;
        scanline2++;
        y2++;
        x3--;
        w3 += 2;

        if(error > 0) { // check if x^2 + y^2 > r^2
            scanlines[scanline3] = (SDL_Rect){x3, y3, w3, 1};
            scanlines[scanline4] = (SDL_Rect){x3, y4, w3, 1};

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            w1 -= 2;
            scanline3++;
            y3++;
            scanline4--;
            y4--;
        }
    }

    if(x == y) {
        scanlines[scanline3] = (SDL_Rect){x3, y3, w3, 1};
        scanlines[scanline4] = (SDL_Rect){x3, y4, w3, 1};
    }

    if(SDL_RenderFillRects(
        a__sdlRenderer, scanlines, (int)A_ARRAY_LEN(scanlines)) < 0) {

        a_out__error("SDL_RenderFillRects: %s", SDL_GetError());
    }
}

APlatformTexture* a_platform__textureScreenNew(int Width, int Height)
{
    SDL_Texture* t = SDL_CreateTexture(a__sdlRenderer,
                                       A_SDL__PIXEL_FORMAT,
                                       SDL_TEXTUREACCESS_TARGET,
                                       Width,
                                       Height);
    if(t == NULL) {
        a_out__fatal("SDL_CreateTexture: %s", SDL_GetError());
    }

    if(SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND) < 0) {
        a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
    }

    APlatformTexture* screen = a_mem_malloc(sizeof(APlatformTexture));

    screen->pixels = NULL;
    screen->pixelsSize = 0;
    screen->w = Width;
    screen->h = Height;
    screen->texture[0] = t;
    screen->texture[1] = NULL;

    return screen;
}

void a_platform__textureSpriteCommit(ASprite* Sprite)
{
    APlatformTexture* texture = Sprite->texture;
    int width = Sprite->w;
    int height = Sprite->h;

    if(texture == NULL) {
        texture = a_mem_zalloc(sizeof(APlatformTexture));
        Sprite->texture = texture;
    }

    if(Sprite->pixelsSize > texture->pixelsSize) {
        free(texture->pixels);

        texture->pixels = a_mem_malloc(Sprite->pixelsSize);
        texture->pixelsSize = Sprite->pixelsSize;
    }

    memcpy(texture->pixels, Sprite->pixels, Sprite->pixelsSize);
    texture->w = Sprite->w;
    texture->h = Sprite->h;

    for(int i = 0; i < A__NUM_SPRITE_TEXTURES; i++) {
        if(texture->texture[i]) {
            SDL_DestroyTexture(texture->texture[i]);
        }

        if(i == 0) {
            for(int i = width * height; i--;) {
                if(Sprite->pixels[i] != a_sprite__colorKey) {
                    // Set full alpha for non-transparent pixel
                    texture->pixels[i] |=
                        (unsigned)A__PIXEL_MASK_ALPHA << A__PIXEL_SHIFT_ALPHA;
                }
            }
        } else if(i == 1) {
            for(int i = width * height; i--;) {
                if(Sprite->pixels[i] != a_sprite__colorKey) {
                    // Set full color for non-transparent pixel
                    texture->pixels[i] |= a_pixel_fromHex(0xffffff);
                }
            }
        }

        SDL_Texture* t = SDL_CreateTexture(a__sdlRenderer,
                                           A_SDL__PIXEL_FORMAT,
                                           SDL_TEXTUREACCESS_TARGET,
                                           width,
                                           height);
        if(t == NULL) {
            a_out__fatal("SDL_CreateTexture: %s", SDL_GetError());
        }

        if(SDL_UpdateTexture(
            t, NULL, texture->pixels, width * (int)sizeof(APixel)) < 0) {

            a_out__fatal("SDL_UpdateTexture: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND) < 0) {
            a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
        }

        texture->texture[i] = t;
    }
}

void a_platform__textureFree(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    for(int i = 0; i < A__NUM_SPRITE_TEXTURES; i++) {
        SDL_DestroyTexture(Texture->texture[i]);
    }

    free(Texture->pixels);
    free(Texture);
}

void a_platform__textureBlit(const APlatformTexture* Texture, int X, int Y, bool FillFlat)
{
    a_platform__textureBlitEx(Texture,
                              X + Texture->w / 2,
                              Y + Texture->h / 2,
                              A_FIX_ONE,
                              0,
                              0,
                              0,
                              FillFlat);
}

void a_platform__textureBlitEx(const APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat)
{
    SDL_Texture* t = Texture->texture[FillFlat];

    if(SDL_SetTextureBlendMode(t, pixelBlendToSdlBlend()) < 0) {
        a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
    }

    if(SDL_SetTextureAlphaMod(t, pixelAlphaToSdlAlpha()) < 0) {
        a_out__error("SDL_SetTextureAlphaMod: %s", SDL_GetError());
    }

    if(FillFlat) {
        if(SDL_SetTextureColorMod(t,
                                  (uint8_t)a_pixel__state.red,
                                  (uint8_t)a_pixel__state.green,
                                  (uint8_t)a_pixel__state.blue) < 0) {

            a_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }

    SDL_Point center = {a_fix_toInt((Texture->w / 2 + CenterX) * Scale),
                        a_fix_toInt((Texture->h / 2 + CenterY) * Scale)};

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     a_fix_toInt(Texture->w * Scale),
                     a_fix_toInt(Texture->h * Scale)};

    if(SDL_RenderCopyEx(a__sdlRenderer,
                        t,
                        NULL,
                        &dest,
                        360 - 360 * Angle / A_FIX_ANGLES_NUM,
                        &center,
                        SDL_FLIP_NONE) < 0) {

        a_out__error("SDL_RenderCopyEx: %s", SDL_GetError());
    }

    if(FillFlat) {
        if(SDL_SetTextureColorMod(t, 0xff, 0xff, 0xff) < 0) {
            a_out__error("SDL_SetTextureColorMod: %s", SDL_GetError());
        }
    }
}

void a_platform__renderTargetSet(APlatformTexture* Texture)
{
    if(SDL_SetRenderTarget(a__sdlRenderer, Texture->texture[0]) < 0) {
        a_out__fatal("SDL_SetRenderTarget: %s", SDL_GetError());
    }
}

void a_platform__renderTargetPixelsGet(APixel* Pixels, int Width)
{
    // Unreliable on texture targets
    if(SDL_RenderReadPixels(a__sdlRenderer,
                            NULL,
                            A_SDL__PIXEL_FORMAT,
                            Pixels,
                            Width * (int)sizeof(APixel)) < 0) {

        a_out__fatal("SDL_RenderReadPixels: %s", SDL_GetError());
    }
}

void a_platform__renderTargetClipSet(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderSetClipRect(a__sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderSetClipRect: %s", SDL_GetError());
    }
}
#endif // A_BUILD_RENDER_SDL
