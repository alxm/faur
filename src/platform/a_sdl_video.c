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

#include <a2x.v.h>

#if A_CONFIG_LIB_SDL
#if A_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
#elif A_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
#endif

#if A_CONFIG_LIB_SDL == 1
    static SDL_Surface* g_sdlScreen;
#elif A_CONFIG_LIB_SDL == 2
    SDL_Renderer* a__sdlRenderer;
    static SDL_Window* g_sdlWindow;
    static ARgb g_clearRgb;

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        static SDL_Texture* g_sdlTexture;
    #endif
#endif

#if A_CONFIG_LIB_SDL == 2 || A_CONFIG_SYSTEM_EMSCRIPTEN
    static bool g_vsync = A_CONFIG_SCREEN_VSYNC;
#else
    static const bool g_vsync = false;
#endif

static AVectorInt g_size = {A_CONFIG_SCREEN_WIDTH, A_CONFIG_SCREEN_HEIGHT};
static bool g_fullscreen = A_CONFIG_SCREEN_FULLSCREEN;
static int g_zoom = A_CONFIG_SCREEN_ZOOM;
static APixels* g_pixels;

void a_platform_sdl_video__init(void)
{
    #if A_CONFIG_SYSTEM_PANDORA
        putenv("SDL_VIDEODRIVER=omapdss");
        putenv("SDL_OMAP_LAYER_SIZE=pixelperfect");
    #endif

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        A__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }
}

void a_platform_sdl_video__uninit(void)
{
    #if A_CONFIG_LIB_SDL == 1
        #if !A_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }
        #endif
    #elif A_CONFIG_LIB_SDL == 2
        #if A_CONFIG_LIB_RENDER_SOFTWARE
            SDL_DestroyTexture(g_sdlTexture);
        #endif

        SDL_DestroyRenderer(a__sdlRenderer);
        SDL_DestroyWindow(g_sdlWindow);
    #endif

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

#if A_CONFIG_LIB_SDL == 1
static bool sdl1ScreenSet(int Width, int Height, uint32_t Flags)
{
    if(SDL_VideoModeOK(Width, Height, A_CONFIG_SCREEN_BPP, Flags) == 0) {
        a_out__error("SDL_VideoModeOK(%d, %d, %d): Mode not available",
                     Width,
                     Height,
                     A_CONFIG_SCREEN_BPP);

        return false;
    }

    g_sdlScreen = SDL_SetVideoMode(Width, Height, A_CONFIG_SCREEN_BPP, Flags);

    if(g_sdlScreen == NULL) {
        A__FATAL("SDL_SetVideoMode(%d, %d, %d): %s",
                 Width,
                 Height,
                 A_CONFIG_SCREEN_BPP,
                 SDL_GetError());
    }

    SDL_SetClipRect(g_sdlScreen, NULL);

    return true;
}
#endif

#if A_CONFIG_SCREEN_HARDWARE_WIDTH > 0 && A_CONFIG_SCREEN_HARDWARE_HEIGHT > 0
static AVectorInt sdlScreenSizeGetNative(void)
{
    return (AVectorInt){A_CONFIG_SCREEN_HARDWARE_WIDTH,
                        A_CONFIG_SCREEN_HARDWARE_HEIGHT};
}
#elif A_CONFIG_LIB_SDL == 1
static AVectorInt sdlScreenSizeGetNative(void)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    return (AVectorInt){info->current_w, info->current_h};
}
#elif A_CONFIG_LIB_SDL == 2
static AVectorInt sdlScreenSizeGetNative(void)
{
    SDL_DisplayMode mode;

    if(SDL_GetCurrentDisplayMode(0, &mode) < 0) {
        a_out__error("SDL_GetCurrentDisplayMode: %s", SDL_GetError());

        return (AVectorInt){0, 0};
    }

    a_out__info("Display info: %dx%d %dbpp",
                mode.w,
                mode.h,
                SDL_BITSPERPIXEL(mode.format));

    return (AVectorInt){mode.w, mode.h};
}
#endif

void a_platform_api__screenInit(void)
{
    if(g_size.x < 0 || g_size.y < 0) {
        AVectorInt res = sdlScreenSizeGetNative();

        if(res.x > 0 && res.y > 0) {
            if(g_size.x < 0) {
                g_size.x = res.x / -g_size.x;
            }

            if(g_size.y < 0) {
                g_size.y = res.y / -g_size.y;
            }
        }
    }

    if(g_size.x <= 0 || g_size.y <= 0) {
        A__FATAL("Invalid screen resolution %dx%d", g_size.x, g_size.y);
    } else {
        a_out__info("Screen resolution %dx%d, zoom x%d",
                    g_size.x,
                    g_size.y,
                    A_CONFIG_SCREEN_ZOOM);
    }

    APixelsFlags pFlags = A_PIXELS__SCREEN;

    #if A_CONFIG_SCREEN_ALLOCATE
        pFlags |= A_PIXELS__ALLOC;
    #endif

    g_pixels = a_pixels__new(g_size.x, g_size.y, pFlags);

    #if A_CONFIG_LIB_SDL == 1
        uint32_t videoFlags = SDL_SWSURFACE;

        #if A_CONFIG_SCREEN_FULLSCREEN
            videoFlags |= SDL_FULLSCREEN;
        #endif

        if(!sdl1ScreenSet(g_size.x * A_CONFIG_SCREEN_ZOOM,
                          g_size.y * A_CONFIG_SCREEN_ZOOM,
                          videoFlags)) {

            A__FATAL("Could not create SDL screen surface");
        }

        #if !A_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            a_pixels__bufferSet(g_pixels,
                                g_sdlScreen->pixels,
                                g_sdlScreen->pitch / sizeof(APixel),
                                g_sdlScreen->h);
        #endif
    #elif A_CONFIG_LIB_SDL == 2
        int ret;
        uint32_t windowFlags = 0;

        #if !A_CONFIG_SYSTEM_EMSCRIPTEN
            windowFlags |= SDL_WINDOW_RESIZABLE;
        #endif

        #if A_CONFIG_SCREEN_MAXIMIZED
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        #endif

        #if A_CONFIG_SCREEN_FULLSCREEN
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        #endif

        g_sdlWindow = SDL_CreateWindow(NULL,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       g_size.x * A_CONFIG_SCREEN_ZOOM,
                                       g_size.y * A_CONFIG_SCREEN_ZOOM,
                                       windowFlags);
        if(g_sdlWindow == NULL) {
            A__FATAL("SDL_CreateWindow: %s", SDL_GetError());
        }

        #if A_CONFIG_SCREEN_FULLSCREEN
            if(SDL_SetWindowFullscreen(
                g_sdlWindow, SDL_WINDOW_FULLSCREEN) < 0) {

                a_out__error("SDL_SetWindowFullscreen: %s", SDL_GetError());
            }
        #endif

        uint32_t rendererFlags = SDL_RENDERER_ACCELERATED
                               | SDL_RENDERER_TARGETTEXTURE;

        #if A_CONFIG_SCREEN_VSYNC
            rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
        #endif

        a__sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, rendererFlags);

        if(a__sdlRenderer == NULL) {
            A__FATAL("SDL_CreateRenderer: %s", SDL_GetError());
        }

        SDL_RendererInfo info;
        SDL_GetRendererInfo(a__sdlRenderer, &info);

        if(!(info.flags & SDL_RENDERER_TARGETTEXTURE)) {
            A__FATAL("SDL_CreateRenderer: "
                     "System does not support SDL_RENDERER_TARGETTEXTURE");
        }

        if(info.flags & SDL_RENDERER_ACCELERATED) {
            a_out__info("Using SDL2 accelerated renderer");
        } else {
            a_out__info("Using SDL2 software renderer");
        }

        g_vsync = info.flags & SDL_RENDERER_PRESENTVSYNC;

        ret = SDL_RenderSetLogicalSize(a__sdlRenderer, g_size.x, g_size.y);

        if(ret < 0) {
            A__FATAL("SDL_RenderSetLogicalSize: %s", SDL_GetError());
        }

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            g_sdlTexture = SDL_CreateTexture(a__sdlRenderer,
                                             A_SDL__PIXEL_FORMAT,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             g_size.x,
                                             g_size.y);

            if(g_sdlTexture == NULL) {
                A__FATAL("SDL_CreateTexture: %s", SDL_GetError());
            }
        #endif

        SDL_SetHintWithPriority(
            SDL_HINT_RENDER_SCALE_QUALITY, "nearest", SDL_HINT_OVERRIDE);

        g_clearRgb = a_pixel_toRgb(
                        a_pixel_fromHex(A_CONFIG_COLOR_SCREEN_BORDER));

        #if A_CONFIG_LIB_RENDER_SDL
            a_pixels__commit(g_pixels);
            a_platform_api__renderTargetSet(g_pixels->texture);
        #endif
    #endif

    a_out__info("V-sync is %s", g_vsync ? "on" : "off");

    #if A_CONFIG_TRAIT_DESKTOP
        const char* caption = a_str__fmt512("%s %s",
                                            A_CONFIG_APP_TITLE,
                                            A_CONFIG_APP_VERSION_STRING);

        #if A_CONFIG_LIB_SDL == 1
            SDL_WM_SetCaption(caption, NULL);
        #elif A_CONFIG_LIB_SDL == 2
            SDL_SetWindowTitle(g_sdlWindow, caption);
        #endif
    #endif

    a_platform_api__screenMouseCursorSet(A_CONFIG_INPUT_MOUSE_CURSOR);

    #if A_CONFIG_SCREEN_WIZ_FIX
        a_platform_wiz__portraitModeSet();
    #endif
}

void a_platform_api__screenUninit(void)
{
    a_pixels__free(g_pixels);
}

#if A_CONFIG_LIB_SDL == 2
void a_platform_api__screenClear(void)
{
    if(SDL_RenderClear(a__sdlRenderer) < 0) {
        a_out__error("SDL_RenderClear: %s", SDL_GetError());
    }
}
#endif

void a_platform_api__screenShow(void)
{
    #if A_CONFIG_LIB_SDL == 1
        #if A_CONFIG_SCREEN_WIZ_FIX
            // The Wiz screen has diagonal tearing in landscape mode. As a slow
            // but simple workaround, the screen is set to portrait mode where
            // 320,0 is top-left and 0,240 is bottom-right, and the game's
            // landscape pixel buffer is rotated to this format every frame.

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            #define A__SCREEN_TOTAL (A_CONFIG_SCREEN_HARDWARE_WIDTH \
                                        * A_CONFIG_SCREEN_HARDWARE_HEIGHT)

            APixel* dst = (APixel*)g_sdlScreen->pixels + A__SCREEN_TOTAL;
            const APixel* src = g_pixels->buffer;

            for(int i = A_CONFIG_SCREEN_HARDWARE_HEIGHT;
                i--;
                dst += A__SCREEN_TOTAL + 1) {

                for(int j = A_CONFIG_SCREEN_HARDWARE_WIDTH; j--; ) {
                    dst -= A_CONFIG_SCREEN_HARDWARE_HEIGHT;
                    *dst = *src++;
                }
            }

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);
        #elif A_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            if(g_zoom <= 1) {
                if(g_sdlScreen->pitch == g_sdlScreen->w * (int)sizeof(APixel)) {
                    a_pixels__copyToBuffer(g_pixels, g_sdlScreen->pixels);
                } else {
                    uint8_t* dst = g_sdlScreen->pixels;
                    const APixel* src = g_pixels->buffer;
                    size_t rowSize = (size_t)g_sdlScreen->w * sizeof(APixel);

                    for(int y = g_sdlScreen->h; y--; ) {
                        memcpy(dst, src, rowSize);

                        dst += g_sdlScreen->pitch;
                        src += g_sdlScreen->w;
                    }
                }
            } else {
                APixel* dst = g_sdlScreen->pixels;
                const APixel* src = g_pixels->buffer;
                int realH = g_sdlScreen->h / g_zoom;
                int realW = g_sdlScreen->w / g_zoom;
                size_t rowLen = g_sdlScreen->pitch / sizeof(APixel);
                ptrdiff_t rowRemainder = (int)rowLen - g_sdlScreen->w;

                for(int y = realH; y--; ) {
                    const APixel* firstLine = dst;

                    for(int x = realW; x--; ) {
                        for(int z = g_zoom; z--; ) {
                            *dst++ = *src;
                        }

                        src++;
                    }

                    dst += rowRemainder;

                    for(int z = g_zoom - 1; z--; ) {
                        memcpy(dst, firstLine, g_sdlScreen->pitch);
                        dst += rowLen;
                    }
                }
            }

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);
        #else
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            a_pixels__bufferSet(g_pixels,
                                g_sdlScreen->pixels,
                                g_sdlScreen->pitch / sizeof(APixel),
                                g_sdlScreen->h);
        #endif
    #elif A_CONFIG_LIB_SDL == 2
        #if A_CONFIG_LIB_RENDER_SDL
            if(SDL_SetRenderTarget(a__sdlRenderer, NULL) < 0) {
                A__FATAL("SDL_SetRenderTarget: %s", SDL_GetError());
            }

            if(SDL_RenderSetClipRect(a__sdlRenderer, NULL) < 0) {
                a_out__error("SDL_RenderSetClipRect: %s", SDL_GetError());
            }
        #endif

        if(SDL_SetRenderDrawColor(a__sdlRenderer,
                                  (uint8_t)g_clearRgb.r,
                                  (uint8_t)g_clearRgb.g,
                                  (uint8_t)g_clearRgb.b,
                                  SDL_ALPHA_OPAQUE) < 0) {

            a_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
        }

        a_platform_api__screenClear();

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            if(SDL_UpdateTexture(g_sdlTexture,
                                 NULL,
                                 g_pixels->buffer,
                                 g_pixels->w * (int)sizeof(APixel)) < 0) {

                A__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
            }

            if(SDL_RenderCopy(a__sdlRenderer, g_sdlTexture, NULL, NULL) < 0) {
                A__FATAL("SDL_RenderCopy: %s", SDL_GetError());
            }
        #else
            a_color_push();
            a_color_blendSet(A_COLOR_BLEND_PLAIN);

            a_platform_api__textureBlit(g_pixels->texture, 0, 0, false);

            a_platform_api__renderTargetSet(g_pixels->texture);
            a_platform_api__renderTargetClipSet(a__screen.clipX,
                                                a__screen.clipY,
                                                a__screen.clipWidth,
                                                a__screen.clipHeight);

            a_color_pop();
        #endif

        SDL_RenderPresent(a__sdlRenderer);
    #endif
}

APixels* a_platform_api__screenPixelsGet(void)
{
    return g_pixels;
}

AVectorInt a_platform_api__screenSizeGet(void)
{
    return g_size;
}

bool a_platform_api__screenVsyncGet(void)
{
    return g_vsync;
}

int a_platform_api__screenZoomGet(void)
{
    return g_zoom;
}

void a_platform_api__screenZoomSet(int Zoom)
{
    #if A_CONFIG_LIB_SDL == 1
        #if A_CONFIG_SCREEN_ALLOCATE
            int newWidth = g_pixels->w * Zoom;
            int newHeight = g_pixels->h * Zoom;

            if(sdl1ScreenSet(newWidth, newHeight, g_sdlScreen->flags)) {
                g_zoom = Zoom;
            }
        #else
            A_UNUSED(Zoom);

            a_out__warning(
                "SDL 1.2 screen zoom needs A_CONFIG_SCREEN_ALLOCATE=1");
        #endif
    #elif A_CONFIG_LIB_SDL == 2
        SDL_SetWindowSize(g_sdlWindow, g_pixels->w * Zoom, g_pixels->h * Zoom);

        g_zoom = Zoom;
    #endif
}

bool a_platform_api__screenFullscreenGet(void)
{
    return g_fullscreen;
}

void a_platform_api__screenFullscreenFlip(void)
{
    g_fullscreen = !g_fullscreen;

    #if A_CONFIG_LIB_SDL == 1
        #if A_CONFIG_SCREEN_ALLOCATE
            uint32_t videoFlags = g_sdlScreen->flags;

            if(g_fullscreen) {
                videoFlags |= SDL_FULLSCREEN;
            } else {
                videoFlags &= ~(uint32_t)SDL_FULLSCREEN;
            }

            sdl1ScreenSet(g_sdlScreen->w, g_sdlScreen->h, videoFlags);
        #else
            a_out__warning(
                "SDL 1.2 fullscreen needs A_CONFIG_SCREEN_ALLOCATE=1");
        #endif
    #elif A_CONFIG_LIB_SDL == 2
        if(SDL_SetWindowFullscreen(
            g_sdlWindow, g_fullscreen ? SDL_WINDOW_FULLSCREEN : 0) < 0) {

            a_out__error("SDL_SetWindowFullscreen: %s", SDL_GetError());
        }
    #endif

    a_platform_api__screenMouseCursorSet(!g_fullscreen);
}

void a_platform_api__screenMouseCursorSet(bool Show)
{
    #if A_CONFIG_INPUT_MOUSE_CURSOR
        int setting = Show ? SDL_ENABLE : SDL_DISABLE;
    #else
        A_UNUSED(Show);
        int setting = SDL_DISABLE;
    #endif

    if(SDL_ShowCursor(setting) < 0) {
        a_out__error("SDL_ShowCursor: %s", SDL_GetError());
    }
}
#endif // A_CONFIG_LIB_SDL
