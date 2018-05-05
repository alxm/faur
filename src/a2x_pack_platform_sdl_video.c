/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_system_includes.h"

#if A_PLATFORM_LIB_SDL
#include "a2x_pack_platform_sdl_video.v.h"

#include <SDL.h>

#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"

#if A_PLATFORM_LIB_SDL == 1
    static SDL_Surface* g_sdlScreen = NULL;
#elif A_PLATFORM_LIB_SDL == 2
    SDL_Renderer* a__sdlRenderer = NULL;
    static SDL_Window* g_sdlWindow = NULL;
    static int g_clearR, g_clearG, g_clearB;

    #if A_PLATFORM_RENDER_SOFTWARE
        static SDL_Texture* g_sdlTexture = NULL;
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
        SDL_DestroyRenderer(a__sdlRenderer);
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

        a__sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, rendererFlags);

        if(a__sdlRenderer == NULL) {
            a_out__fatal("SDL_CreateRenderer failed: %s", SDL_GetError());
        }

        if(rendererFlags & SDL_RENDERER_PRESENTVSYNC) {
            SDL_RendererInfo info;
            SDL_GetRendererInfo(a__sdlRenderer, &info);

            if(!(info.flags & SDL_RENDERER_PRESENTVSYNC))  {
                a_out__warning("Cannot use vsync");
                a_settings__set("video.vsync", "0");
            }
        }

        ret = SDL_RenderSetLogicalSize(a__sdlRenderer,
                                       Width,
                                       Height);
        if(ret < 0) {
            a_out__fatal("SDL_RenderSetLogicalSize failed: %s", SDL_GetError());
        }

        #if A_PLATFORM_RENDER_SOFTWARE
            g_sdlTexture = SDL_CreateTexture(a__sdlRenderer,
                                             A_SDL__PIXEL_FORMAT,
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

        a_pixel_toRgb(a_settings_getPixel("video.color.border"),
                      &g_clearR,
                      &g_clearG,
                      &g_clearB);
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
            if(SDL_SetRenderTarget(a__sdlRenderer, NULL) < 0) {
                a_out__fatal("SDL_SetRenderTarget failed: %s", SDL_GetError());
            }

            if(SDL_RenderSetClipRect(a__sdlRenderer, NULL) < 0) {
                a_out__error("SDL_RenderSetClipRect failed: %s",
                             SDL_GetError());
            }
        #endif

        if(SDL_SetRenderDrawColor(a__sdlRenderer,
                                  (uint8_t)g_clearR,
                                  (uint8_t)g_clearG,
                                  (uint8_t)g_clearB,
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

            if(SDL_RenderCopy(a__sdlRenderer, g_sdlTexture, NULL, NULL) < 0) {
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

        SDL_RenderPresent(a__sdlRenderer);
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
    if(SDL_RenderClear(a__sdlRenderer) < 0) {
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
#elif A_PLATFORM_LIB_SDL_GETFULLRES
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
#endif // A_PLATFORM_LIB_SDL
