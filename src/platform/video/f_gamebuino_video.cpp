/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

extern "C" {
    #include "f_gamebuino_video.v.h"
}

#include <faur.v.h>

#if F_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

static FScreen g_screen;
static FPixels g_pixels, g_lights;

static const FVectorInt g_screenSize = {F_CONFIG_SCREEN_HARDWARE_WIDTH,
                                        F_CONFIG_SCREEN_HARDWARE_HEIGHT};
static const FVectorInt g_lightsSize = {2, 4};

void f_platform_api__screenInit(void)
{
    f_pixels__init(
        &g_pixels, g_screenSize.x, g_screenSize.y, 1, (FPixelsFlags)0);
    f_pixels__bufferSet(
        &g_pixels, gb.display._buffer, g_screenSize.x, g_screenSize.y);

    f_pixels__init(
        &g_lights, g_lightsSize.x, g_lightsSize.y, 1, (FPixelsFlags)0);
    f_pixels__bufferSet(
        &g_lights, gb.lights._buffer, g_lightsSize.x, g_lightsSize.y);
}

void f_platform_api__screenUninit(void)
{
}

void f_platform_api__screenClear(void)
{
}

void f_platform_api__screenShow(void)
{
}

bool f_platform_api__screenVsyncGet(void)
{
    return true;
}

FVectorInt f_platform_api__screenSizeGet(void)
{
    return g_screenSize;
}

FPixels* f_platform_api__screenPixelsGet(void)
{
    return &g_pixels;
}

void f_gamebuino_lightsStart(void)
{
    g_screen = f__screen;

    f__screen.pixels = &g_lights;
    f__screen.sprite = NULL;
    f__screen.frame = 0;
    f__screen.clipX = 0;
    f__screen.clipY = 0;
    f__screen.clipX2 = g_lightsSize.x;
    f__screen.clipY2 = g_lightsSize.y;
    f__screen.clipWidth = g_lightsSize.x;
    f__screen.clipHeight = g_lightsSize.y;
}

void f_gamebuino_lightsEnd(void)
{
    f__screen = g_screen;
}
#endif // F_CONFIG_SYSTEM_GAMEBUINO
