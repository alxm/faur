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
    const APixels* pixels;
    SDL_Texture* texture[A_TEXTURE__NUM];
};

extern SDL_Renderer* a__sdlRenderer;

static inline SDL_BlendMode pixelBlendToSdlBlend(void)
{
    switch(a__color.blend) {
        case A_COLOR_BLEND_MOD:
            return SDL_BLENDMODE_MOD;

        case A_COLOR_BLEND_ADD:
            return SDL_BLENDMODE_ADD;

        default:
            return SDL_BLENDMODE_BLEND;
    }
}

static inline uint8_t pixelAlphaToSdlAlpha(void)
{
    switch(a__color.blend) {
        case A_COLOR_BLEND_RGBA:
            return (uint8_t)a__color.alpha;

        case A_COLOR_BLEND_RGB25:
            return SDL_ALPHA_OPAQUE / 4;

        case A_COLOR_BLEND_RGB50:
            return SDL_ALPHA_OPAQUE / 2;

        case A_COLOR_BLEND_RGB75:
            return SDL_ALPHA_OPAQUE * 3 / 4;

        default:
            return SDL_ALPHA_OPAQUE;
    }
}

void a_platform_api__renderSetDrawColor(void)
{
    if(SDL_SetRenderDrawColor(a__sdlRenderer,
                              (uint8_t)a__color.rgb.r,
                              (uint8_t)a__color.rgb.g,
                              (uint8_t)a__color.rgb.b,
                              pixelAlphaToSdlAlpha()) < 0) {

        a_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
    }
}

void a_platform_api__renderSetBlendMode(void)
{
    if(SDL_SetRenderDrawBlendMode(a__sdlRenderer, pixelBlendToSdlBlend()) < 0) {
        a_out__error("SDL_SetRenderDrawBlendMode: %s", SDL_GetError());
    }
}

void a_platform_api__drawPixel(int X, int Y)
{
    if(SDL_RenderDrawPoint(a__sdlRenderer, X, Y) < 0) {
        a_out__error("SDL_RenderDrawPoint: %s", SDL_GetError());
    }
}

void a_platform_api__drawLine(int X1, int Y1, int X2, int Y2)
{
    if(SDL_RenderDrawLine(a__sdlRenderer, X1, Y1, X2, Y2) < 0) {
        a_out__error("SDL_RenderDrawLine: %s", SDL_GetError());
    }
}

void a_platform_api__drawHLine(int X1, int X2, int Y)
{
    a_platform_api__drawRectangleFilled(X1, Y, X2 - X1 + 1, 1);
}

void a_platform_api__drawVLine(int X, int Y1, int Y2)
{
    a_platform_api__drawRectangleFilled(X, Y1, 1, Y2 - Y1 + 1);
}

void a_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderFillRect(a__sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderFillRect: %s", SDL_GetError());
    }
}

void a_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    a_platform_api__drawRectangleFilled(X, Y, Width, 1);

    if(Height <= 1) {
        return;
    }

    a_platform_api__drawRectangleFilled(X, Y + Height - 1, Width, 1);

    if(Width <= 1 || Height <= 2) {
        return;
    }

    a_platform_api__drawRectangleFilled(X, Y + 1, 1, Height - 2);
    a_platform_api__drawRectangleFilled(X + Width - 1, Y + 1, 1, Height - 2);
}

void a_platform_api__drawCircleOutline(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            a_platform_api__drawRectangleFilled(X - 1, Y - 1, 2, 2);
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

void a_platform_api__drawCircleFilled(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            a_platform_api__drawRectangleFilled(X - 1, Y - 1, 2, 2);
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

APlatformTexture* a_platform_api__textureNew(const APixels* Pixels)
{
    APlatformTexture* texture = a_mem_zalloc(sizeof(APlatformTexture));

    texture->pixels = Pixels;

    if(A_FLAG_TEST_ANY(Pixels->flags, A_PIXELS__SCREEN)) {
        texture->texture[A_TEXTURE__NORMAL] = SDL_CreateTexture(
                                                a__sdlRenderer,
                                                A_SDL__PIXEL_FORMAT,
                                                SDL_TEXTUREACCESS_TARGET,
                                                Pixels->w,
                                                Pixels->h);

        if(texture->texture[A_TEXTURE__NORMAL] == NULL) {
            A__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(
            texture->texture[A_TEXTURE__NORMAL], SDL_BLENDMODE_BLEND) < 0) {

            a_out__error("SDL_SetTextureBlendMode: %s", SDL_GetError());
        }
    } else {
        APixel* buffer = a_mem_dup(Pixels->buffer, Pixels->bufferSize);

        for(int t = 0; t < A_TEXTURE__NUM; t++) {
            switch(t) {
                case A_TEXTURE__NORMAL: {
                    for(int i = Pixels->w * Pixels->h; i--; ) {
                        if(Pixels->buffer[i] != a_color__key) {
                            // Set full alpha for non-transparent pixel
                            buffer[i] |= (APixel)A__PX_MASK_A << A__PX_SHIFT_A;
                        }
                    }
                } break;

                case A_TEXTURE__COLORMOD_BITMAP: {
                    for(int i = Pixels->w * Pixels->h; i--; ) {
                        if(Pixels->buffer[i] == a_color__key) {
                            // Set full color for transparent pixel
                            buffer[i] |= a_pixel_fromHex(0xffffff);
                        }
                    }
                } break;

                case A_TEXTURE__COLORMOD_FLAT: {
                    for(int i = Pixels->w * Pixels->h; i--;) {
                        if(Pixels->buffer[i] != a_color__key) {
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

        free(buffer);
    }

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

    free(Texture);
}

void a_platform_api__textureBlit(const APlatformTexture* Texture, int X, int Y)
{
    a_platform_api__textureBlitEx(Texture,
                                  X + Texture->pixels->w / 2,
                                  Y + Texture->pixels->h / 2,
                                  A_FIX_ONE,
                                  0,
                                  0,
                                  0);
}

void a_platform_api__textureBlitEx(const APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY)
{
    SDL_Texture* tex;
    SDL_BlendMode blend = pixelBlendToSdlBlend();

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

    if(SDL_SetTextureAlphaMod(tex, pixelAlphaToSdlAlpha()) < 0) {
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

    SDL_Point center = {
        a_fix_toInt((Texture->pixels->w / 2 + CenterX) * Scale),
        a_fix_toInt((Texture->pixels->h / 2 + CenterY) * Scale)
    };

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     a_fix_toInt(Texture->pixels->w * Scale),
                     a_fix_toInt(Texture->pixels->h * Scale)};

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

void a_platform_api__renderTargetSet(APlatformTexture* Texture)
{
    if(SDL_SetRenderTarget(
        a__sdlRenderer, Texture->texture[A_TEXTURE__NORMAL]) < 0) {

        A__FATAL("SDL_SetRenderTarget: %s", SDL_GetError());
    }
}

void a_platform_api__renderTargetPixelsCapture(APixel* Pixels, int Width)
{
    // Unreliable on texture targets
    if(SDL_RenderReadPixels(a__sdlRenderer,
                            NULL,
                            A_SDL__PIXEL_FORMAT,
                            Pixels,
                            Width * (int)sizeof(APixel)) < 0) {

        A__FATAL("SDL_RenderReadPixels: %s", SDL_GetError());
    }
}

void a_platform_api__renderTargetClipSet(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderSetClipRect(a__sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderSetClipRect: %s", SDL_GetError());
    }
}
#endif // A_CONFIG_LIB_RENDER_SDL
