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

#define F__NEOPIXELS_W 2
#define F__NEOPIXELS_H 4

static FScreen g_screen;
static const FVecInt g_lightsSize = {F__NEOPIXELS_W, F__NEOPIXELS_H};

#if F_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

static FPixels g_pixels, g_lights;
static const FVecInt g_screenSize = {F_CONFIG_SCREEN_SIZE_WIDTH_HW,
                                     F_CONFIG_SCREEN_SIZE_HEIGHT_HW};

void f_platform_api__screenInit(void)
{
    f_pixels__init(&g_pixels, g_screenSize.x, g_screenSize.y, 1, 0);
    f_pixels__bufferSet(
        &g_pixels, gb.display._buffer, g_screenSize.x, g_screenSize.y);

    f_pixels__init(&g_lights, g_lightsSize.x, g_lightsSize.y, 1, 0);
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

FVecInt f_platform_api__screenSizeGet(void)
{
    return g_screenSize;
}

FPixels* f_platform_api__screenPixelsGet(void)
{
    return &g_pixels;
}

int f_platform_api__screenZoomGet(void)
{
    return F_CONFIG_SCREEN_SIZE_ZOOM;
}

bool f_platform_api__screenFullscreenGet(void)
{
    return F_CONFIG_SCREEN_FULLSCREEN;
}
#else // !F_CONFIG_SYSTEM_GAMEBUINO
static FColorPixel g_buffer[F__NEOPIXELS_W * F__NEOPIXELS_H] = {0};
static FPixels g_lights = {
    {F__NEOPIXELS_W, F__NEOPIXELS_H},
    1,
    F__NEOPIXELS_W * F__NEOPIXELS_H,
    F__NEOPIXELS_W * F__NEOPIXELS_H * sizeof(FColorPixel),
    0,
    g_buffer,
};
#endif // !F_CONFIG_SYSTEM_GAMEBUINO

void f_gamebuino_lightsStart(void)
{
    g_screen = f__screen;

    f__screen.pixels = &g_lights;
    f__screen.sprite = NULL;
    f__screen.frame = 0;
    f__screen.clipStart.x = 0;
    f__screen.clipStart.y = 0;
    f__screen.clipEnd = g_lightsSize;
    f__screen.clipSize = g_lightsSize;

    f_color_push();
}

void f_gamebuino_lightsEnd(void)
{
    f__screen = g_screen;

    f_color_pop();
}
