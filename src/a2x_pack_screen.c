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

#define A_SCREEN_SIZE (a_width * a_height * sizeof(Pixel))

Pixel* a_pixels = NULL;
int a_width = 0;
int a_height = 0;

static Pixel* a__pixels2 = NULL;
static int a__width2 = 0;
static int a__height2 = 0;

static Sprite* spriteTarget = NULL;

static void setFakeScreen(void);
static void displayVolume(void);

void a_screen__init(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    a_width = a2x_int("video.width");
    a_height = a2x_int("video.height");

    #if !A_PLATFORM_LINUXPC
        a2x__set("video.scale", "1");
    #endif

    if(a2x_int("video.scale") > 1) {
        a2x__set("video.fake", "1");
    }

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

            a2x__set("video.fake", "1");
        #else
            a2x__set("video.wizTear", "0");
        #endif
    }

    if(a2x_bool("video.fake")) {
        setFakeScreen();
    } else {
        a_sdl__screen_lock();

        a_pixels = a_sdl__screen_pixels();
        a__pixels2 = a_pixels;
    }

    a__width2 = a_width;
    a__height2 = a_height;
}

void a_screen__uninit(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    if(a2x_bool("video.fake")) {
        free(a_pixels);
    } else {
        a_sdl__screen_unlock();
    }
}

void a_screen_show(void)
{
    displayVolume();
    a_console__draw();

    if(a2x_bool("video.wizTear")) {
        // video.fake is also set when video.wizTear is set

        a_sdl__screen_lock();

        #define A_WIDTH 320
        #define A_HEIGHT 240

        Pixel* dst = a_sdl__screen_pixels() + A_WIDTH * A_HEIGHT;
        const Pixel* src = a_pixels;

        for(int i = A_HEIGHT; i--; dst += A_WIDTH * A_HEIGHT + 1) {
            for(int j = A_WIDTH; j--; ) {
                dst -= A_HEIGHT;
                *dst = *src++;
            }
        }

        a_sdl__screen_unlock();
        a_sdl__screen_flip();
    } else if(a2x_bool("video.fake")) {
        a_sdl__screen_lock();

        switch(a2x_int("video.scale")) {
            case 1: {
                const Pixel* const src = a_pixels;
                Pixel* const dst = a_sdl__screen_pixels();

                memcpy(dst, src, A_SCREEN_SIZE);
            } break;

            case 2: {
                const Pixel* src = a_pixels;
                uint32_t* dst = (uint32_t*)a_sdl__screen_pixels();

                const int len = a_width;
                const int size = len * sizeof(uint32_t);

                for(int i = a_height; i--; ) {
                    for(int j = len; j--; ) {
                        const Pixel p = *src++;
                        *dst++ = (p << 16) | p;
                    }

                    memcpy(dst, dst - len, size);
                    dst += len;
                }
            } break;

            case 3: {
                const Pixel* src = a_pixels;
                Pixel* dst = a_sdl__screen_pixels();

                const int width = a_width;
                const int scaledWidth = width * 3;
                const int scaledSize = scaledWidth * sizeof(Pixel);

                for(int i = a_height; i--; ) {
                    for(int j = width; j--; ) {
                        const Pixel p = *src++;
                        *dst++ = p;
                        *dst++ = p;
                        *dst++ = p;
                    }

                    for(int j = 2; j--; ) {
                        memcpy(dst, dst - scaledWidth, scaledSize);
                        dst += scaledWidth;
                    }
                }
            } break;
        }

        a_sdl__screen_unlock();
        a_sdl__screen_flip();
    } else {
        a_sdl__screen_unlock();
        a_sdl__screen_flip();
        a_sdl__screen_lock();

        a_pixels = a_sdl__screen_pixels();
        a__pixels2 = a_pixels;
    }
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
        a_sprite_refresh(spriteTarget);
        spriteTarget = NULL;
    }
}

#if A_PLATFORM_LINUXPC || A_PLATFORM_WINDOWS
    bool a_screen__change(void)
    {
        if(!a2x_bool("video.fake")) {
            a_sdl__screen_unlock();
        }

        bool changed = a_sdl__screen_set();

        if(!a2x_bool("video.fake")) {
            a_sdl__screen_lock();
        }

        if(changed) {
            if(a2x_int("video.scale") > 1) {
                // once we scale up, we switch to fake screen
                a2x__set("video.fake", "1");
            }

            if(a2x_bool("video.fake")) {
                setFakeScreen();
            } else {
                a_pixels = a_sdl__screen_pixels();
                a__pixels2 = a_pixels;
            }

            a_out__message("Changed resolution to %dx%d %dx (%s)",
                  a_width, a_height, a2x_int("video.scale"),
                  a2x_bool("video.fullscreen") ? "fullscreen" : "windowed");
        } else {
            a_out__warning("Could not change resolution to %dx%d %dx (%s)",
                      a_width, a_height, a2x_int("video.scale"),
                      a2x_bool("video.fullscreen") ? "fullscreen" : "windowed");
        }

        return changed;
    }
#endif

static void setFakeScreen(void)
{
    a_pixels = a_mem_malloc(A_SCREEN_SIZE);
    a__pixels2 = a_pixels;

    memset(a_pixels, 0, A_SCREEN_SIZE);
}

static void displayVolume(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a2x_bool("sound.on")) {
            if(a_time_getMilis() - a__volumeAdjust > A_MILIS_VOLUME) {
                return;
            }

            a_pixel_setBlend(A_PIXEL_PLAIN);

            a_pixel_setPixel(0);
            a_draw_rectangle(0, 181, A_MAX_VOLUME / A_VOLUME_STEP + 5, 197);

            a_pixel_setRGB(255, 156, 107);
            a_draw_hline(0, A_MAX_VOLUME / A_VOLUME_STEP + 4 + 1, 180);
            a_draw_hline(0, A_MAX_VOLUME / A_VOLUME_STEP + 4 + 1, 183 + 14);
            a_draw_vline(A_MAX_VOLUME / A_VOLUME_STEP + 4 + 1, 181, 183 + 14);

            a_pixel_setRGB(255, 84, 0);
            a_draw_rectangle(0, 186, a__volume / A_VOLUME_STEP, 192);
        }
    #endif
}
