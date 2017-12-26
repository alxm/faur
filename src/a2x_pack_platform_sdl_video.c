/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_platform_sdl_video.v.h"

#include <SDL.h>

#if A_PLATFORM_LIB_SDL == 1
    static SDL_Surface* g_sdlScreen = NULL;
#elif A_PLATFORM_LIB_SDL == 2
    static SDL_Window* g_sdlWindow = NULL;
    static SDL_Renderer* g_sdlRenderer = NULL;
    static uint8_t g_clearR, g_clearG, g_clearB;

    #if A_PLATFORM_RENDER_SOFTWARE
        static SDL_Texture* g_sdlTexture = NULL;
    #elif A_PLATFORM_RENDER_SDL
        #define NUM_SPRITE_TEXTURES 2

        struct APlatformTexture {
            int width, height;
            APixel* pixels;
            SDL_Texture* texture[NUM_SPRITE_TEXTURES];
        };
    #endif

    #if A_PIXEL__BPP == 16
        #if A_PLATFORM_RENDER_SOFTWARE
            #define A_PIXEL_FORMAT SDL_PIXELFORMAT_RGB565
        #elif A_PLATFORM_RENDER_SDL
            #define A_PIXEL_FORMAT SDL_PIXELFORMAT_RGBA5551
        #endif
    #elif A_PIXEL__BPP == 32
        #if A_PLATFORM_RENDER_SOFTWARE
            #define A_PIXEL_FORMAT SDL_PIXELFORMAT_RGBX8888
        #elif A_PLATFORM_RENDER_SDL
            #define A_PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
        #endif
    #endif
#endif

void a_platform_sdl_video__init(void)
{
    #if A_PLATFORM_SYSTEM_PANDORA
        putenv("SDL_VIDEODRIVER=omapdss");
        putenv("SDL_OMAP_LAYER_SIZE=pixelperfect");
    #endif

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }
}

void a_platform_sdl_video__uninit(void)
{
    #if A_PLATFORM_LIB_SDL == 1
        if(!a_settings_getBool("video.doubleBuffer")) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }
        }
    #elif A_PLATFORM_LIB_SDL == 2
        #if A_PLATFORM_RENDER_SOFTWARE
            SDL_DestroyTexture(g_sdlTexture);
        #endif
        SDL_DestroyRenderer(g_sdlRenderer);
        SDL_DestroyWindow(g_sdlWindow);
    #endif

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void a_platform__setScreen(int Width, int Height, bool FullScreen)
{
    #if A_PLATFORM_LIB_SDL == 1
        int bpp = 0;
        uint32_t videoFlags = SDL_SWSURFACE;

        if(FullScreen) {
            videoFlags |= SDL_FULLSCREEN;
        }

        bpp = SDL_VideoModeOK(Width,
                              Height,
                              A_PIXEL__BPP,
                              videoFlags);
        if(bpp == 0) {
            a_out__fatal("SDL: %dx%d:%d video not available",
                         Width,
                         Height,
                         A_PIXEL__BPP);
        }

        g_sdlScreen = SDL_SetVideoMode(Width,
                                       Height,
                                       A_PIXEL__BPP,
                                       videoFlags);
        if(g_sdlScreen == NULL) {
            a_out__fatal("SDL: %s", SDL_GetError());
        }

        SDL_SetClipRect(g_sdlScreen, NULL);

        if(!a_settings_getBool("video.doubleBuffer")) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    a_out__fatal("SDL_LockSurface failed: %s", SDL_GetError());
                }
            }
        }

        a__screen.pixels = g_sdlScreen->pixels;
    #elif A_PLATFORM_LIB_SDL == 2
        int ret;
        uint32_t windowFlags = SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE;

        if(FullScreen) {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        g_sdlWindow = SDL_CreateWindow("",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       Width,
                                       Height,
                                       windowFlags);
        if(g_sdlWindow == NULL) {
            a_out__fatal("SDL_CreateWindow failed: %s", SDL_GetError());
        }

        uint32_t rendererFlags = SDL_RENDERER_ACCELERATED
                               | SDL_RENDERER_TARGETTEXTURE;

        if(a_settings_getBool("video.vsync")) {
            rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
        }

        g_sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, rendererFlags);

        if(g_sdlRenderer == NULL) {
            a_out__fatal("SDL_CreateRenderer failed: %s", SDL_GetError());
        }

        if(rendererFlags & SDL_RENDERER_PRESENTVSYNC) {
            SDL_RendererInfo info;
            SDL_GetRendererInfo(g_sdlRenderer, &info);

            if(!(info.flags & SDL_RENDERER_PRESENTVSYNC))  {
                a_out__warning("Cannot use vsync");
                a_settings__set("video.vsync", "0");
            }
        }

        ret = SDL_RenderSetLogicalSize(g_sdlRenderer,
                                       Width,
                                       Height);
        if(ret < 0) {
            a_out__fatal("SDL_RenderSetLogicalSize failed: %s", SDL_GetError());
        }

        #if A_PLATFORM_RENDER_SOFTWARE
            g_sdlTexture = SDL_CreateTexture(g_sdlRenderer,
                                             A_PIXEL_FORMAT,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             Width,
                                             Height);
            if(g_sdlTexture == NULL) {
                a_out__fatal("SDL_CreateTexture failed: %s", SDL_GetError());
            }
        #endif

        SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY,
                                "nearest",
                                SDL_HINT_OVERRIDE);

        APixel color = a_settings_getPixel("video.color.border");
        g_clearR = (uint8_t)a_pixel_red(color);
        g_clearG = (uint8_t)a_pixel_green(color);
        g_clearB = (uint8_t)a_pixel_blue(color);
    #endif

    #if A_PLATFORM_SYSTEM_DESKTOP || A_PLATFORM_SYSTEM_EMSCRIPTEN
        char caption[64];
        snprintf(caption,
                 sizeof(caption),
                 "%s %s",
                 a_settings_getString("app.title"),
                 a_settings_getString("app.version"));

        #if A_PLATFORM_LIB_SDL == 1
            SDL_WM_SetCaption(caption, NULL);
        #elif A_PLATFORM_LIB_SDL == 2
            SDL_SetWindowTitle(g_sdlWindow, caption);
        #endif
    #endif
}

void a_platform__showScreen(void)
{
    #if A_PLATFORM_LIB_SDL == 1
        #if A_PLATFORM_SYSTEM_WIZ
            if(a_settings_getBool("video.fixWizTearing")) {
                // The Wiz screen has diagonal tearing in landscape mode. As a
                // slow but simple workaround, the screen is set to portrait
                // mode where top-right is 0,0 and bottom-left is 240,320, and
                // the game's landscape pixel buffer is rotated to this format
                // every frame.

                #define A_WIDTH 320
                #define A_HEIGHT 240

                if(SDL_MUSTLOCK(g_sdlScreen)) {
                    if(SDL_LockSurface(g_sdlScreen) < 0) {
                        a_out__fatal("SDL_LockSurface failed: %s",
                                     SDL_GetError());
                    }
                }

                APixel* dst = (APixel*)g_sdlScreen->pixels + A_WIDTH * A_HEIGHT;
                const APixel* src = a__screen.pixels;

                for(int i = A_HEIGHT; i--; dst += A_WIDTH * A_HEIGHT + 1) {
                    for(int j = A_WIDTH; j--; ) {
                        dst -= A_HEIGHT;
                        *dst = *src++;
                    }
                }

                if(SDL_MUSTLOCK(g_sdlScreen)) {
                    SDL_UnlockSurface(g_sdlScreen);
                }

                SDL_Flip(g_sdlScreen);
                return;
            }
        #endif

        if(a_settings_getBool("video.doubleBuffer")) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    a_out__fatal("SDL_LockSurface failed: %s",
                                 SDL_GetError());
                }
            }

            memcpy(g_sdlScreen->pixels, a__screen.pixels, a__screen.pixelsSize);

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);
        } else {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    a_out__fatal("SDL_LockSurface failed: %s",
                                 SDL_GetError());
                }
            }

            a__screen.pixels = g_sdlScreen->pixels;
        }
    #elif A_PLATFORM_LIB_SDL == 2
        #if A_PLATFORM_RENDER_SDL
            if(SDL_SetRenderTarget(g_sdlRenderer, NULL) < 0) {
                a_out__fatal("SDL_SetRenderTarget failed: %s", SDL_GetError());
            }

            if(SDL_RenderSetClipRect(g_sdlRenderer, NULL) < 0) {
                a_out__error("SDL_RenderSetClipRect failed: %s",
                             SDL_GetError());
            }
        #endif

        if(SDL_SetRenderDrawColor(g_sdlRenderer,
                                  g_clearR,
                                  g_clearG,
                                  g_clearB,
                                  SDL_ALPHA_OPAQUE) < 0) {

            a_out__error("SDL_SetRenderDrawColor failed: %s", SDL_GetError());
        }

        a_platform__renderClear();

        #if A_PLATFORM_RENDER_SOFTWARE
            if(SDL_UpdateTexture(g_sdlTexture,
                                 NULL,
                                 a__screen.pixels,
                                 a__screen.width * (int)sizeof(APixel)) < 0) {

                a_out__fatal("SDL_UpdateTexture failed: %s", SDL_GetError());
            }

            if(SDL_RenderCopy(g_sdlRenderer, g_sdlTexture, NULL, NULL) < 0) {
                a_out__fatal("SDL_RenderCopy failed: %s", SDL_GetError());
            }
        #else
            a_pixel_push();
            a_pixel_setBlend(A_PIXEL_BLEND_PLAIN);

            a_platform__blitTexture(a__screen.texture, 0, 0, false);

            a_platform__setRenderTarget(a__screen.texture);
            a_platform__setTargetClip(a__screen.clipX,
                                      a__screen.clipY,
                                      a__screen.clipWidth,
                                      a__screen.clipHeight);

            a_pixel_pop();
        #endif

        SDL_RenderPresent(g_sdlRenderer);
    #endif
}

void a_platform__setFullScreen(bool FullScreen)
{
    #if A_PLATFORM_LIB_SDL == 2
        if(SDL_SetWindowFullscreen(g_sdlWindow,
                                   FullScreen? SDL_WINDOW_FULLSCREEN : 0) < 0) {

            a_out__error("SDL_SetWindowFullscreen failed: %s", SDL_GetError());
        }
    #endif

    int toggle = FullScreen || a_settings_getBool("input.hideCursor")
                    ? SDL_DISABLE
                    : SDL_ENABLE;

    if(SDL_ShowCursor(toggle) < 0) {
        a_out__error("SDL_ShowCursor failed: %s", SDL_GetError());
    }
}

#if A_PLATFORM_LIB_SDL == 2
void a_platform__renderClear(void)
{
    if(SDL_RenderClear(g_sdlRenderer) < 0) {
        a_out__error("SDL_RenderClear failed: %s", SDL_GetError());
    }
}

void a_platform__getNativeResolution(int* Width, int* Height)
{
    SDL_DisplayMode mode;

    if(SDL_GetCurrentDisplayMode(0, &mode) < 0) {
        a_out__error("SDL_GetCurrentDisplayMode failed: %s",
                     SDL_GetError());
        return;
    }

    a_out__message("Display info: %dx%d %dbpp",
                   mode.w,
                   mode.h,
                   SDL_BITSPERPIXEL(mode.format));

    if(*Width < 0) {
        *Width = mode.w;
    }

    if(*Height < 0) {
        *Height = mode.h;
    }
}
#elif !(A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ || A_PLATFORM_SYSTEM_CAANOO)
void a_platform__getNativeResolution(int* Width, int* Height)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    if(*Width < 0) {
        *Width = info->current_w;
    }

    if(*Height < 0) {
        *Height = info->current_h;
    }
}
#endif

#if A_PLATFORM_RENDER_SDL
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
    if(SDL_SetRenderDrawColor(g_sdlRenderer,
                              (uint8_t)a_pixel__state.red,
                              (uint8_t)a_pixel__state.green,
                              (uint8_t)a_pixel__state.blue,
                              pixelAlphaToSdlAlpha()) < 0) {

        a_out__error("SDL_SetRenderDrawColor failed: %s", SDL_GetError());
    }
}

void a_platform__renderSetBlendMode(void)
{
    if(SDL_SetRenderDrawBlendMode(g_sdlRenderer, pixelBlendToSdlBlend()) < 0) {
        a_out__error("SDL_SetRenderDrawBlendMode failed: %s", SDL_GetError());
    }
}

void a_platform__drawPixel(int X, int Y)
{
    if(SDL_RenderDrawPoint(g_sdlRenderer, X, Y) < 0) {
        a_out__error("SDL_RenderDrawPoint failed: %s", SDL_GetError());
    }
}

void a_platform__drawLine(int X1, int Y1, int X2, int Y2)
{
    if(SDL_RenderDrawLine(g_sdlRenderer, X1, Y1, X2, Y2) < 0) {
        a_out__error("SDL_RenderDrawLine failed: %s", SDL_GetError());
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

    if(SDL_RenderFillRect(g_sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderFillRect failed: %s", SDL_GetError());
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

    if(SDL_RenderDrawPoints(g_sdlRenderer,
                            (SDL_Point*)scanlines,
                            numPointPairs * 4 * 2) < 0) {

        a_out__error("SDL_RenderDrawPoints failed: %s", SDL_GetError());
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

    if(SDL_RenderFillRects(g_sdlRenderer,
                           scanlines,
                           (int)A_ARRAY_LEN(scanlines)) < 0) {

        a_out__error("SDL_RenderFillRects failed: %s", SDL_GetError());
    }
}

APlatformTexture* a_platform__newScreenTexture(int Width, int Height)
{
    SDL_Texture* t = SDL_CreateTexture(g_sdlRenderer,
                                       A_PIXEL_FORMAT,
                                       SDL_TEXTUREACCESS_TARGET,
                                       Width,
                                       Height);
    if(t == NULL) {
        a_out__fatal("SDL_CreateTexture failed: %s", SDL_GetError());
    }

    if(SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND) < 0) {
        a_out__error("SDL_SetTextureBlendMode failed: %s", SDL_GetError());
    }

    APlatformTexture* screen = a_mem_malloc(sizeof(APlatformTexture));

    screen->width = Width;
    screen->height = Height;
    screen->texture[0] = t;
    screen->texture[1] = NULL;
    screen->pixels = NULL;

    return screen;
}

APlatformTexture* a_platform__newSpriteTexture(APlatformTexture* Texture, const APixel* Pixels, int Width, int Height)
{
    size_t pixelsSize = (unsigned)Width * (unsigned)Height * sizeof(APixel);

    if(Texture == NULL || Width > Texture->width) {
        a_platform__freeTexture(Texture);
        Texture = a_mem_zalloc(sizeof(APlatformTexture));

        Texture->width = Width;
        Texture->height = Height;
        Texture->pixels = a_mem_dup(Pixels, pixelsSize);
    } else {
        memcpy(Texture->pixels, Pixels, pixelsSize);
    }

    for(int i = 0; i < NUM_SPRITE_TEXTURES; i++) {
        if(Texture->texture[i] != NULL) {
            SDL_DestroyTexture(Texture->texture[i]);
        }

        if(i == 0) {
            for(int i = Width * Height; i--;) {
                if(Pixels[i] != a_sprite__colorKey) {
                    // Set full alpha for non-transparent pixel
                    Texture->pixels[i] |= A_PIXEL__MASK_ALPHA;
                }
            }
        } else if(i == 1) {
            for(int i = Width * Height; i--;) {
                if(Pixels[i] != a_sprite__colorKey) {
                    // Set full color for non-transparent pixel
                    Texture->pixels[i] |= a_pixel_hex(0xffffff);
                }
            }
        }

        SDL_Texture* t = SDL_CreateTexture(g_sdlRenderer,
                                           A_PIXEL_FORMAT,
                                           SDL_TEXTUREACCESS_TARGET,
                                           Width,
                                           Height);
        if(t == NULL) {
            a_out__fatal("SDL_CreateTexture failed: %s", SDL_GetError());
        }

        if(SDL_UpdateTexture(t,
                             NULL,
                             Texture->pixels,
                             Width * (int)sizeof(APixel)) < 0) {

            a_out__fatal("SDL_UpdateTexture failed: %s", SDL_GetError());
        }

        if(SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND) < 0) {
            a_out__error("SDL_SetTextureBlendMode failed: %s", SDL_GetError());
        }

        Texture->texture[i] = t;
    }

    return Texture;
}

void a_platform__freeTexture(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    for(int i = 0; i < NUM_SPRITE_TEXTURES; i++) {
        SDL_DestroyTexture(Texture->texture[i]);
    }

    free(Texture->pixels);
    free(Texture);
}

void a_platform__blitTexture(APlatformTexture* Texture, int X, int Y, bool FillFlat)
{
    a_platform__blitTextureEx(Texture,
                              X + Texture->width / 2,
                              Y + Texture->height / 2,
                              A_FIX_ONE,
                              0,
                              0,
                              0,
                              FillFlat);
}

void a_platform__blitTextureEx(APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat)
{
    SDL_Texture* t = Texture->texture[FillFlat];

    if(SDL_SetTextureBlendMode(t, pixelBlendToSdlBlend()) < 0) {
        a_out__error("SDL_SetTextureBlendMode failed: %s", SDL_GetError());
    }

    if(SDL_SetTextureAlphaMod(t, pixelAlphaToSdlAlpha()) < 0) {
        a_out__error("SDL_SetTextureAlphaMod failed: %s", SDL_GetError());
    }

    if(FillFlat) {
        if(SDL_SetTextureColorMod(t,
                                  (uint8_t)a_pixel__state.red,
                                  (uint8_t)a_pixel__state.green,
                                  (uint8_t)a_pixel__state.blue) < 0) {

            a_out__error("SDL_SetTextureColorMod failed: %s", SDL_GetError());
        }
    }

    SDL_Point center = {a_fix_fixtoi((Texture->width / 2 + CenterX) * Scale),
                        a_fix_fixtoi((Texture->height / 2 + CenterY) * Scale)};

    SDL_Rect dest = {X - center.x,
                     Y - center.y,
                     a_fix_fixtoi(Texture->width * Scale),
                     a_fix_fixtoi(Texture->height * Scale)};

    if(SDL_RenderCopyEx(g_sdlRenderer,
                        t,
                        NULL,
                        &dest,
                        360 - 360 * Angle / A_MATH_ANGLES_NUM,
                        &center,
                        SDL_FLIP_NONE) < 0) {

        a_out__error("SDL_RenderCopyEx failed: %s", SDL_GetError());
    }

    if(FillFlat) {
        if(SDL_SetTextureColorMod(t, 0xff, 0xff, 0xff) < 0) {
            a_out__error("SDL_SetTextureColorMod failed: %s", SDL_GetError());
        }
    }
}

void a_platform__setRenderTarget(APlatformTexture* Texture)
{
    if(SDL_SetRenderTarget(g_sdlRenderer, Texture->texture[0]) < 0) {
        a_out__fatal("SDL_SetRenderTarget failed: %s", SDL_GetError());
    }
}

void a_platform__getTargetPixels(APixel* Pixels, int Width)
{
    // Unreliable on texture targets
    if(SDL_RenderReadPixels(g_sdlRenderer,
                            NULL,
                            A_PIXEL_FORMAT,
                            Pixels,
                            Width * (int)sizeof(APixel)) < 0) {

        a_out__fatal("SDL_RenderReadPixels failed: %s", SDL_GetError());
    }
}

void a_platform__setTargetClip(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderSetClipRect(g_sdlRenderer, &area) < 0) {
        a_out__error("SDL_RenderSetClipRect failed: %s", SDL_GetError());
    }
}
#endif // A_PLATFORM_RENDER_SDL
