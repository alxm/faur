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

extern FCallApi_TextureNew f_platform_api_software__textureNew;
extern FCallApi_TextureDup f_platform_api_software__textureDup;
extern FCallApi_TextureFree f_platform_api_software__textureFree;

extern FCallApi_TextureUpdate f_platform_api_software__textureUpdate;
extern FCallApi_TextureBlit f_platform_api_software__textureBlit;
extern FCallApi_TextureBlitEx f_platform_api_software__textureBlitEx;

#endif // F_INC_PLATFORM_GRAPHICS_SOFTWARE_BLIT_V_H
