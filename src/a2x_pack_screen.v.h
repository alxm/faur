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

#pragma once

#include "a2x_pack_screen.p.h"

typedef void (*AScreenOverlay)(void);

#include "a2x_pack_draw.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_platform_sdl.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_sprite.v.h"

struct AScreen {
    APixel* pixels;
    size_t pixelsSize;
    #if A_CONFIG_RENDER_SOFTWARE
        ASprite* sprite;
    #else
        APlatformTexture* texture;
    #endif
    int width;
    int height;
    int clipX;
    int clipY;
    int clipX2;
    int clipY2;
    int clipWidth;
    int clipHeight;
    bool ownsBuffer;
};

extern AScreen a__screen;

extern void a_screen__init(void);
extern void a_screen__uninit(void);

extern void a_screen__show(void);
extern void a_screen__addOverlay(AScreenOverlay Callback);

extern bool a_screen__sameSize(const AScreen* Screen1, const AScreen* Screen2);
