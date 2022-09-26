/*
    Copyright 2017 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_GRAPHICS_SDL_BLIT_V_H
#define F_INC_PLATFORM_GRAPHICS_SDL_BLIT_V_H

#include "f_sdl_blit.p.h"

#include "../f_platform.v.h"

extern FPlatformTextureScreen* f_platform_api_sdl__textureSpriteToScreen(FPlatformTexture* SpriteTexture);

extern FPlatformTexture* f_platform_api_sdl__textureNew(const FPixels* Pixels);
extern FPlatformTexture* f_platform_api_sdl__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels);
extern void f_platform_api_sdl__textureFree(FPlatformTexture* Texture);

extern void f_platform_api_sdl__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
extern void f_platform_api_sdl__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);

#endif // F_INC_PLATFORM_GRAPHICS_SDL_BLIT_V_H
