/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#if A_CONFIG_LIB_SDL
#include <SDL.h>

#include "a2x_pack_main.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform_wiz.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_str.v.h"

#if A_CONFIG_LIB_SDL == 1
    static SDL_Surface* g_sdlScreen;
#elif A_CONFIG_LIB_SDL == 2
    SDL_Renderer* a__sdlRenderer;
    static SDL_Window* g_sdlWindow;
    static int g_clearR, g_clearG, g_clearB;

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        static SDL_Texture* g_sdlTexture;
    #endif
#endif

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
        if(!a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }
        }
    #elif A_CONFIG_LIB_SDL == 2
        #if A_CONFIG_LIB_RENDER_SOFTWARE
            SDL_DestroyTexture(g_sdlTexture);
        #endif
        SDL_DestroyRenderer(a__sdlRenderer);
        SDL_DestroyWindow(g_sdlWindow);
    #endif

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

#if A_CONFIG_LIB_SDL == 2
static void settingBorderColor(ASettingId Setting)
{
    a_pixel_toRgb(
        a_settings_colorGet(Setting), &g_clearR, &g_clearG, &g_clearB);
}
#endif

static void settingFullscreen(ASettingId Setting)
{
    bool fullScreen = a_settings_boolGet(Setting);

    #if A_CONFIG_LIB_SDL == 1
        if(a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            uint32_t videoFlags = g_sdlScreen->flags;

            if(fullScreen) {
                videoFlags |= SDL_FULLSCREEN;
            } else {
                videoFlags &= ~(uint32_t)SDL_FULLSCREEN;
            }

            g_sdlScreen = SDL_SetVideoMode(0, 0, 0, videoFlags);

            if(g_sdlScreen == NULL) {
                A__FATAL("SDL_SetVideoMode: %s", SDL_GetError());
            }

            SDL_SetClipRect(g_sdlScreen, NULL);
        } else {
            a_out__warning(
                "SDL 1.2 fullscreen needs %s=true",
                a_settings__idToString(A_SETTING_VIDEO_DOUBLEBUFFER));
        }
    #elif A_CONFIG_LIB_SDL == 2
        if(SDL_SetWindowFullscreen(
            g_sdlWindow, fullScreen? SDL_WINDOW_FULLSCREEN : 0) < 0) {

            a_out__error("SDL_SetWindowFullscreen: %s", SDL_GetError());
        }
    #endif

    a_settings_boolSet(A_SETTING_INPUT_MOUSE_CURSOR, !fullScreen);
}

static void settingVideoZoom(ASettingId Setting)
{
    int w = a__screen.width;
    int h = a__screen.height;
    int zoom = a_settings_intGet(Setting);

    #if A_CONFIG_LIB_SDL == 1
        if(a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            g_sdlScreen = SDL_SetVideoMode(
                            w * zoom, h * zoom, 0, g_sdlScreen->flags);

            if(g_sdlScreen == NULL) {
                A__FATAL("SDL_SetVideoMode: %s", SDL_GetError());
            }

            SDL_SetClipRect(g_sdlScreen, NULL);
        } else {
            a_out__warning(
                "SDL 1.2 zoom needs %s=true",
                a_settings__idToString(A_SETTING_VIDEO_DOUBLEBUFFER));
        }
    #elif A_CONFIG_LIB_SDL == 2
        SDL_SetWindowSize(g_sdlWindow, w * zoom, h * zoom);
    #endif
}

static void settingMouseCursor(ASettingId Setting)
{
    int toggle = a_settings_boolGet(Setting) ? SDL_ENABLE : SDL_DISABLE;

    if(SDL_ShowCursor(toggle) < 0) {
        a_out__error("SDL_ShowCursor: %s", SDL_GetError());
    }
}

void a_platform__screenInit(int Width, int Height)
{
    int zoom = a_settings_intGet(A_SETTING_VIDEO_ZOOM);
    bool fullscreen = a_settings_boolGet(A_SETTING_VIDEO_FULLSCREEN);

    #if A_CONFIG_LIB_SDL == 1
        int bpp = 0;
        uint32_t videoFlags = SDL_SWSURFACE;

        if(fullscreen) {
            videoFlags |= SDL_FULLSCREEN;
        }

        bpp = SDL_VideoModeOK(Width, Height, A__PIXEL_BPP, videoFlags);

        if(bpp == 0) {
            A__FATAL("SDL_VideoModeOK: %dx%d:%d video not available",
                     Width,
                     Height,
                     A__PIXEL_BPP);
        }

        g_sdlScreen = SDL_SetVideoMode(
                        Width * zoom, Height * zoom, A__PIXEL_BPP, videoFlags);

        if(g_sdlScreen == NULL) {
            A__FATAL("SDL_SetVideoMode: %s", SDL_GetError());
        }

        SDL_SetClipRect(g_sdlScreen, NULL);

        if(!a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            a__screen.pixels = g_sdlScreen->pixels;
        }
    #elif A_CONFIG_LIB_SDL == 2
        int ret;
        uint32_t windowFlags = SDL_WINDOW_RESIZABLE;

        if(a_settings_boolGet(A_SETTING_VIDEO_MAX_WINDOW)) {
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        }

        if(fullscreen) {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        g_sdlWindow = SDL_CreateWindow("",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       Width * zoom,
                                       Height * zoom,
                                       windowFlags);
        if(g_sdlWindow == NULL) {
            A__FATAL("SDL_CreateWindow: %s", SDL_GetError());
        }

        uint32_t rendererFlags = SDL_RENDERER_ACCELERATED
                               | SDL_RENDERER_TARGETTEXTURE;

        if(a_settings_boolGet(A_SETTING_VIDEO_VSYNC)) {
            rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
        }

        a__sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, rendererFlags);

        if(a__sdlRenderer == NULL) {
            A__FATAL("SDL_CreateRenderer: %s", SDL_GetError());
        }

        if(rendererFlags & SDL_RENDERER_PRESENTVSYNC) {
            SDL_RendererInfo info;
            SDL_GetRendererInfo(a__sdlRenderer, &info);

            if(!(info.flags & SDL_RENDERER_PRESENTVSYNC)) {
                a_out__warning("Cannot use vsync");
                a_settings_boolSet(A_SETTING_VIDEO_VSYNC, false);
            }
        }

        ret = SDL_RenderSetLogicalSize(a__sdlRenderer, Width, Height);

        if(ret < 0) {
            A__FATAL("SDL_RenderSetLogicalSize: %s", SDL_GetError());
        }

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            g_sdlTexture = SDL_CreateTexture(a__sdlRenderer,
                                             A_SDL__PIXEL_FORMAT,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             Width,
                                             Height);
            if(g_sdlTexture == NULL) {
                A__FATAL("SDL_CreateTexture: %s", SDL_GetError());
            }
        #endif

        SDL_SetHintWithPriority(
            SDL_HINT_RENDER_SCALE_QUALITY, "nearest", SDL_HINT_OVERRIDE);

        a_settings__callbackSet(
            A_SETTING_COLOR_SCREEN_BORDER, settingBorderColor, true);
    #endif

    #if A_CONFIG_TRAIT_DESKTOP
        const char* caption = a_str__fmt512(
                                "%s %s",
                                A_CONFIG_APP_NAME,
                                A_CONFIG_APP_VERSION);

        #if A_CONFIG_LIB_SDL == 1
            SDL_WM_SetCaption(caption, NULL);
        #elif A_CONFIG_LIB_SDL == 2
            SDL_SetWindowTitle(g_sdlWindow, caption);
        #endif
    #endif

    a_settings__callbackSet(
        A_SETTING_INPUT_MOUSE_CURSOR, settingMouseCursor, true);

    #if A_CONFIG_LIB_SDL == 1
        a_settings__callbackSet(
            A_SETTING_VIDEO_FULLSCREEN, settingFullscreen, false);
    #elif A_CONFIG_LIB_SDL == 2
        a_settings__callbackSet(
            A_SETTING_VIDEO_FULLSCREEN, settingFullscreen, true);
    #endif

    a_settings__callbackSet(A_SETTING_VIDEO_ZOOM, settingVideoZoom, false);

    #if A_CONFIG_SYSTEM_WIZ
        if(a_settings_boolGet(A_SETTING_SYSTEM_WIZ_FIXTEARING)) {
            a_platform_wiz__portraitModeSet();
        }
    #endif
}

void a_platform__screenShow(void)
{
    #if A_CONFIG_LIB_SDL == 1
        #if A_CONFIG_SYSTEM_WIZ
            if(a_settings_boolGet(A_SETTING_SYSTEM_WIZ_FIXTEARING)) {
                // The Wiz screen has diagonal tearing in landscape mode. As a
                // slow but simple workaround, the screen is set to portrait
                // mode where 320,0 is top-left and 0,240 is bottom-right, and
                // the game's landscape pixel buffer is rotated to this format
                // every frame.

                #define A__WIDTH 320
                #define A__HEIGHT 240

                if(SDL_MUSTLOCK(g_sdlScreen)) {
                    if(SDL_LockSurface(g_sdlScreen) < 0) {
                        A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                    }
                }

                APixel* dst = (APixel*)g_sdlScreen->pixels
                                + A__WIDTH * A__HEIGHT;
                const APixel* src = a__screen.pixels;

                for(int i = A__HEIGHT; i--; dst += A__WIDTH * A__HEIGHT + 1) {
                    for(int j = A__WIDTH; j--; ) {
                        dst -= A__HEIGHT;
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

        if(a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            int zoom = a_settings_intGet(A_SETTING_VIDEO_ZOOM);

            if(zoom <= 1) {
                memcpy(g_sdlScreen->pixels,
                       a__screen.pixels,
                       a__screen.pixelsSize);
            } else {
                int realH = g_sdlScreen->h / zoom;
                int realW = g_sdlScreen->w / zoom;

                APixel* dst = (APixel*)g_sdlScreen->pixels;
                const APixel* srcStart = a__screen.pixels;

                ptrdiff_t dstRemainderInc =
                    (int)g_sdlScreen->pitch / (int)sizeof(APixel)
                        - g_sdlScreen->w;
                ptrdiff_t srcStartInc = g_sdlScreen->w / zoom;

                for(int y = realH; y--; ) {
                    for(int z = zoom; z--; ) {
                        const APixel* src = srcStart;

                        for(int x = realW; x--; ) {
                            for(int z = zoom; z--; ) {
                                *dst++ = *src;
                            }

                            src++;
                        }

                        dst += dstRemainderInc;
                    }

                    srcStart += srcStartInc;
                }
            }

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
                    A__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            a__screen.pixels = g_sdlScreen->pixels;
        }
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
                                  (uint8_t)g_clearR,
                                  (uint8_t)g_clearG,
                                  (uint8_t)g_clearB,
                                  SDL_ALPHA_OPAQUE) < 0) {

            a_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
        }

        a_platform__renderClear();

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            if(SDL_UpdateTexture(g_sdlTexture,
                                 NULL,
                                 a__screen.pixels,
                                 a__screen.width * (int)sizeof(APixel)) < 0) {

                A__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
            }

            if(SDL_RenderCopy(a__sdlRenderer, g_sdlTexture, NULL, NULL) < 0) {
                A__FATAL("SDL_RenderCopy: %s", SDL_GetError());
            }
        #else
            a_pixel_push();
            a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);

            a_platform__textureBlit(a__screen.texture, 0, 0, false);

            a_platform__renderTargetSet(a__screen.texture);
            a_platform__renderTargetClipSet(a__screen.clipX,
                                      a__screen.clipY,
                                      a__screen.clipWidth,
                                      a__screen.clipHeight);

            a_pixel_pop();
        #endif

        SDL_RenderPresent(a__sdlRenderer);
    #endif
}

#if A_CONFIG_LIB_SDL == 2
void a_platform__renderClear(void)
{
    if(SDL_RenderClear(a__sdlRenderer) < 0) {
        a_out__error("SDL_RenderClear: %s", SDL_GetError());
    }
}
#endif

#if A_CONFIG_SCREEN_HARDWARE_WIDTH > 0 && A_CONFIG_SCREEN_HARDWARE_HEIGHT > 0
void a_platform__screenResolutionGetNative(int* Width, int* Height)
{
    *Width = A_CONFIG_SCREEN_HARDWARE_WIDTH;
    *Height = A_CONFIG_SCREEN_HARDWARE_HEIGHT;
}
#elif A_CONFIG_LIB_SDL == 2
void a_platform__screenResolutionGetNative(int* Width, int* Height)
{
    SDL_DisplayMode mode;

    if(SDL_GetCurrentDisplayMode(0, &mode) < 0) {
        a_out__error("SDL_GetCurrentDisplayMode: %s", SDL_GetError());
        return;
    }

    a_out__message("Display info: %dx%d %dbpp",
                   mode.w,
                   mode.h,
                   SDL_BITSPERPIXEL(mode.format));

    *Width = mode.w;
    *Height = mode.h;
}
#elif A_CONFIG_LIB_SDL == 1
void a_platform__screenResolutionGetNative(int* Width, int* Height)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    *Width = info->current_w;
    *Height = info->current_h;
}
#endif
#endif // A_CONFIG_LIB_SDL
