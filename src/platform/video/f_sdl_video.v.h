/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_VIDEO_SDL_VIDEO_V_H
#define F_INC_PLATFORM_VIDEO_SDL_VIDEO_V_H

#include "f_sdl_video.p.h"

#include "../f_platform.v.h"
#include "../../graphics/f_color.v.h"

#if F_CONFIG_LIB_SDL == 2
    #if F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_RGB_565
        #define F_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGB565
    #elif F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_RGBA_5551
        #define F_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGBA5551
    #elif F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_RGBA_8888
        #define F_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
    #elif F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_ARGB_8888
        #define F_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888
    #elif F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_ABGR_8888
        #define F_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_ABGR8888
    #else
        #error FAUR_ERROR: Invalid F_CONFIG_SCREEN_FORMAT
    #endif
#endif

extern void f_platform_sdl_video__init(void);
extern void f_platform_sdl_video__uninit(void);

extern FCallApi_ScreenInit f_platform_api_sdl___screenInit;
extern FCallApi_ScreenUninit f_platform_api_sdl__screenUninit;

extern FCallApi_ScreenClear f_platform_api_sdl__screenClear;

extern FCallApi_ScreenTextureGet f_platform_api_sdl__screenTextureGet;
extern FCallApi_ScreenTextureSet f_platform_api_sdl__screenTextureSet;
extern FCallApi_ScreenTextureSync f_platform_api_sdl__screenTextureSync;
extern FCallApi_ScreenToTexture f_platform_api_sdl__screenToTexture;

extern FCallApi_ScreenClipSet f_platform_api_sdl__screenClipSet;
extern FCallApi_ScreenShow f_platform_api_sdl__screenShow;

extern FCallApi_ScreenPixelsGet f_platform_api_sdl__screenPixelsGet;
extern FCallApi_ScreenSizeGet f_platform_api_sdl__screenSizeGet;

extern FCallApi_ScreenVsyncGet f_platform_api_sdl__screenVsyncGet;

extern FCallApi_ScreenZoomGet f_platform_api_sdl__screenZoomGet;
extern FCallApi_ScreenZoomSet f_platform_api_sdl__screenZoomSet;

extern FCallApi_ScreenFullscreenGet f_platform_api_sdl__screenFullscreenGet;
extern FCallApi_ScreenFullscreenFlip f_platform_api_sdl__screenFullscreenFlip;

extern int f_platform_sdl_video__pixelBlendToSdlBlend(void);
extern uint8_t f_platform_sdl_video__pixelAlphaToSdlAlpha(void);

#endif // F_INC_PLATFORM_VIDEO_SDL_VIDEO_V_H
