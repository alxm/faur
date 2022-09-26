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

#ifndef F_INC_PLATFORM_GRAPHICS_SOFTWARE_BLIT_V_H
#define F_INC_PLATFORM_GRAPHICS_SOFTWARE_BLIT_V_H

#include "f_software_blit.p.h"

#if F_CONFIG_TRAIT_LOW_MEM
    typedef uint8_t FSpriteWord;
#else
    typedef unsigned FSpriteWord;
#endif

#include "../f_platform.v.h"

extern void f_platform_software_blit__init(void);
extern void f_platform_software_blit__uninit(void);

extern FPlatformTexture* f_platform_api_software__textureNew(const FPixels* Pixels);
extern FPlatformTexture* f_platform_api_software__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels);
extern void f_platform_api_software__textureFree(FPlatformTexture* Texture);

extern void f_platform_api_software__textureUpdate(FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame);
extern void f_platform_api_software__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
extern void f_platform_api_software__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);

#endif // F_INC_PLATFORM_GRAPHICS_SOFTWARE_BLIT_V_H
