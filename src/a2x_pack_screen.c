/*
    Copyright 2010 Alex Margarit

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

/*
    Many thanks:

    - to Orkie for the Wiz framebuffer direction code
*/

#include "a2x_pack_screen.v.h"

APixel* a_screen__pixels = NULL;
int a_screen__width = 0;
int a_screen__height = 0;

APixel* a_screen__savedPixels = NULL;
static int g_savedWidth = 0;
static int g_savedHeight = 0;

static ASprite* g_spriteTarget = NULL;

static AList* g_overlays;

void a_screen__init(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    a_screen__width = a_settings_getInt("video.width");
    a_screen__height = a_settings_getInt("video.height");

    if(a_settings_getBool("video.window")) {
        a_sdl__screen_set();

        #if A_PLATFORM_WIZ
            if(a_settings_getBool("video.fixWizTearing")) {
                #define FBIO_MAGIC 'D'
                #define FBIO_LCD_CHANGE_CONTROL \
                    _IOW(FBIO_MAGIC, 90, unsigned int[2])
                #define LCD_DIRECTION_ON_CMD 5 // 320x240
                #define LCD_DIRECTION_OFF_CMD 6 // 240x320

                unsigned int send[2];
                int fb_fd = open("/dev/fb0", O_RDWR);
                send[0] = LCD_DIRECTION_OFF_CMD;
                ioctl(fb_fd, FBIO_LCD_CHANGE_CONTROL, &send);
                close(fb_fd);
            }
        #endif
    }

    if(a_settings_getBool("video.doubleBuffer")) {
        a_screen__pixels = a_screen_new();
    }

    g_savedWidth = a_screen__width;
    g_savedHeight = a_screen__height;
    a_screen__savedPixels = a_screen__pixels;

    g_overlays = a_list_new();
}

void a_screen__uninit(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    if(a_settings_getBool("video.doubleBuffer")) {
        // Use savedPixels in case app forgot to call a_screen_resetTarget
        free(a_screen__savedPixels);
    }

    a_list_free(g_overlays);
}

APixel* a_screen_pixels(void)
{
    return a_screen__pixels;
}

int a_screen_width(void)
{
    return a_screen__width;
}

int a_screen_height(void)
{
    return a_screen__height;
}

void a_screen_show(void)
{
    if(a_screen__pixels != a_screen__savedPixels) {
        a_out__fatal("Must call a_screen_resetTarget before drawing frame");
    }

    A_LIST_ITERATE(g_overlays, AScreenOverlay, callback) {
        callback();
    }

    a_sdl__screen_show();
}

APixel* a_screen_dup(void)
{
    APixel* const dst = a_screen_new();
    a_screen_copy(dst, a_screen__pixels);
    return dst;
}

APixel* a_screen_new(void)
{
    APixel* buffer = a_mem_malloc(A_SCREEN_SIZE);
    memset(buffer, 0, A_SCREEN_SIZE);
    return buffer;
}

void a_screen_copy(APixel* Dst, const APixel* Src)
{
    memcpy(Dst, Src, A_SCREEN_SIZE);
}

void a_screen_copyPart(APixel* Dst, int X, int Y, int Width, int Height)
{
    const APixel* screen = a_screen__pixels + Y * a_screen__width + X;

    for(int i = Height; i--; ) {
        memcpy(Dst, screen, Width * sizeof(APixel));
        Dst += Width;
        screen += a_screen__width;
    }
}

void a_screen_setTarget(APixel* Pixels, int Width, int Height)
{
    a_screen__pixels = Pixels;
    a_screen__width = Width;
    a_screen__height = Height;
}

void a_screen_setTargetSprite(ASprite* Sprite)
{
    a_screen__pixels = Sprite->pixels;
    a_screen__width = Sprite->w;
    a_screen__height = Sprite->h;

    g_spriteTarget = Sprite;
}

void a_screen_resetTarget(void)
{
    a_screen__pixels = a_screen__savedPixels;
    a_screen__width = g_savedWidth;
    a_screen__height = g_savedHeight;

    if(g_spriteTarget) {
        g_spriteTarget = NULL;
    }
}

void a_screen__addOverlay(AScreenOverlay Callback)
{
    a_list_addLast(g_overlays, Callback);
}
