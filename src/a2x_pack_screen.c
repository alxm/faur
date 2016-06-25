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

Pixel* a_pixels = NULL;
int a_width = 0;
int a_height = 0;

Pixel* a__pixels2 = NULL;
static int a__width2 = 0;
static int a__height2 = 0;

static Sprite* spriteTarget = NULL;

static void displayVolume(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a2x_bool("sound.on")) {
            if(a_time_getMilis() - a__volumeAdjust > A_MILIS_VOLUME) {
                return;
            }

            a_pixel_setBlend(A_PIXEL_PLAIN);

            a_pixel_setPixel(0);
            a_draw_rectangle(0, 181, a__volumeMax / A_VOLUME_STEP + 5, 197);

            a_pixel_setRGB(255, 156, 107);
            a_draw_hline(0, a__volumeMax / A_VOLUME_STEP + 4 + 1, 180);
            a_draw_hline(0, a__volumeMax / A_VOLUME_STEP + 4 + 1, 183 + 14);
            a_draw_vline(a__volumeMax / A_VOLUME_STEP + 4 + 1, 181, 183 + 14);

            a_pixel_setRGB(255, 84, 0);
            a_draw_rectangle(0, 186, a__volume / A_VOLUME_STEP, 192);
        }
    #endif
}

void a_screen__init(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    a_width = a2x_int("video.width");
    a_height = a2x_int("video.height");

    a_sdl__screen_set();

    if(a2x_bool("video.wizTear")) {
        #if A_PLATFORM_WIZ
            #define FBIO_MAGIC 'D'
            #define FBIO_LCD_CHANGE_CONTROL _IOW(FBIO_MAGIC, 90, unsigned int[2])
            #define LCD_DIRECTION_ON_CMD 5 // 320x240
            #define LCD_DIRECTION_OFF_CMD 6 // 240x320

            unsigned int send[2];
            int fb_fd = open("/dev/fb0", O_RDWR);
            send[0] = LCD_DIRECTION_OFF_CMD;
            ioctl(fb_fd, FBIO_LCD_CHANGE_CONTROL, &send);
            close(fb_fd);

            a2x__set("video.doubleBuffer", "1");
        #else
            a2x__set("video.wizTear", "0");
        #endif
    }

    if(a2x_bool("video.doubleBuffer")) {
        a_pixels = a_mem_malloc(A_SCREEN_SIZE);
        memset(a_pixels, 0, A_SCREEN_SIZE);
    }

    a__width2 = a_width;
    a__height2 = a_height;
    a__pixels2 = a_pixels;
}

void a_screen__uninit(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    if(a2x_bool("video.doubleBuffer")) {
        free(a_pixels);
    }
}

void a_screen_show(void)
{
    displayVolume();
    a_console__draw();

    a_sdl__screen_show();
}

Pixel* a_screen_dup(void)
{
    Pixel* const dst = a_screen_new();
    a_screen_copy(dst, a_pixels);

    return dst;
}

Pixel* a_screen_new(void)
{
    return a_mem_malloc(A_SCREEN_SIZE);
}

void a_screen_copy(Pixel* dst, const Pixel* src)
{
    memcpy(dst, src, A_SCREEN_SIZE);
}

void a_screen_copyPart(Pixel* dst, int x, int y, int w, int h)
{
    const Pixel* screen = a_pixels + y * a_width + x;

    for(int i = h; i--; ) {
        memcpy(dst, screen, w * sizeof(Pixel));
        dst += w;
        screen += a_width;
    }
}

void a_screen_setTarget(Pixel* p, int w, int h)
{
    a_pixels = p;
    a_width = w;
    a_height = h;
}

void a_screen_setTargetSprite(Sprite* s)
{
    a_pixels = s->data;
    a_width = s->w;
    a_height = s->h;

    spriteTarget = s;
}

void a_screen_resetTarget(void)
{
    a_pixels = a__pixels2;
    a_width = a__width2;
    a_height = a__height2;

    if(spriteTarget) {
        spriteTarget = NULL;
    }
}
