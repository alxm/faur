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

#if A_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

static APixels g_pixels;
static const AVectorInt g_size = {A_CONFIG_SCREEN_HARDWARE_WIDTH,
                                  A_CONFIG_SCREEN_HARDWARE_HEIGHT};

void f_platform_api__screenInit(void)
{
    f_pixels__init(&g_pixels, g_size.x, g_size.y, 1, (APixelsFlags)0);
    f_pixels__bufferSet(&g_pixels, gb.display._buffer, g_size.x, g_size.y);
}

void f_platform_api__screenUninit(void)
{
}

void f_platform_api__screenShow(void)
{
}

bool f_platform_api__screenVsyncGet(void)
{
    return true;
}

AVectorInt f_platform_api__screenSizeGet(void)
{
    return g_size;
}

APixels* f_platform_api__screenPixelsGet(void)
{
    return &g_pixels;
}
#endif // A_CONFIG_SYSTEM_GAMEBUINO
