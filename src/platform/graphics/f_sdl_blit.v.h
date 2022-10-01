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

extern FCallApi_TextureNew f_platform_api_sdl__textureNew;
extern FCallApi_TextureDup f_platform_api_sdl__textureDup;
extern FCallApi_TextureFree f_platform_api_sdl__textureFree;

extern FCallApi_TextureBlit f_platform_api_sdl__textureBlit;
extern FCallApi_TextureBlitEx f_platform_api_sdl__textureBlitEx;

#endif // F_INC_PLATFORM_GRAPHICS_SDL_BLIT_V_H
