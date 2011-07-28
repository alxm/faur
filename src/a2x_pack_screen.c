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

#if A_PLATFORM_GP2X
    //#include "../gp2x/flush_uppermem_cache.h"
#endif

#define A_SCREEN_SIZE (a_width * a_height * sizeof(Pixel))

Pixel* a_pixels;
int a_width;
int a_height;

static SDL_Surface* a_screen;

static Pixel* a__pixels2;
static int a__width2;
static int a__height2;

static uint32_t videoFlags;

static void setSDLScreen(void);
static void setFakeScreen(void);
static void displayVolume(void);

void a_screen__set(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    a_width = a2x_int("video.width");
    a_height = a2x_int("video.height");

    videoFlags = SDL_SWSURFACE;

    #if !A_PLATFORM_LINUXPC
        a2x__set("video.double", "0");
    #endif

    if(a2x_bool("video.double")) {
        a2x__set("video.fake", "1");
    }

    setSDLScreen();

    #if A_PLATFORM_LINUXPC
        String64 caption;
        sprintf(caption, "%s %s", a2x_str("app.title"), a2x_str("app.version"));
        SDL_WM_SetCaption(caption, NULL);
    #else
        SDL_ShowCursor(SDL_DISABLE);
    #endif

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
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_LockSurface(a_screen);
        }

        a_pixels = a_screen->pixels;
    }

    a__pixels2 = a_pixels;
    a__width2 = a_width;
    a__height2 = a_height;
}

void a_screen__free(void)
{
    if(!a2x_bool("video.window")) {
        return;
    }

    if(a2x_bool("video.fake")) {
        free(a_pixels);
    } else {
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_UnlockSurface(a_screen);
        }
    }
}

void a_screen_show(void)
{
    displayVolume();

    if(a2x_bool("video.wizTear")) {
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_LockSurface(a_screen);
        }

        #define A_WIDTH 320
        #define A_HEIGHT 240

        Pixel* dst = (Pixel*)a_screen->pixels + A_WIDTH * A_HEIGHT;
        const Pixel* src = a_pixels;

        for(int i = A_HEIGHT; i--; dst += A_WIDTH * A_HEIGHT + 1) {
            for(int j = A_WIDTH; j--; ) {
                dst -= A_HEIGHT;
                *dst = *src++;
            }
        }

        if(SDL_MUSTLOCK(a_screen)) {
            SDL_UnlockSurface(a_screen);
        }

        SDL_Flip(a_screen);
    } else if(a2x_bool("video.fake")) {
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_LockSurface(a_screen);
        }

        if(a2x_bool("video.double")) {
            const Pixel* src = a_pixels;
            uint32_t* dst = (uint32_t*)a_screen->pixels;

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
        } else {
            const Pixel* const src = a_pixels;
            Pixel* const dst = a_screen->pixels;

            memcpy(dst, src, A_SCREEN_SIZE);
        }

        if(SDL_MUSTLOCK(a_screen)) {
            SDL_UnlockSurface(a_screen);
        }

        SDL_Flip(a_screen);
    } else {
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_UnlockSurface(a_screen);
        }

        SDL_Flip(a_screen);

        if(SDL_MUSTLOCK(a_screen)) {
            SDL_LockSurface(a_screen);
        }
    }

    #if A_PLATFORM_GP2X
        //flush_uppermem_cache(a_screen->pixels, a_screen->pixels + WIDTH * HEIGHT, 0);
    #endif
}

Pixel* a_screen_dup(void)
{
    Pixel* const dst = malloc(A_SCREEN_SIZE);
    a_screen_copy(dst, a_pixels);

    return dst;
}

void a_screen_copy(Pixel* const dst, const Pixel* const src)
{
    memcpy(dst, src, A_SCREEN_SIZE);
}

void a_screen_copyPart(Pixel* dst, const int x, const int y, const int w, const int h)
{
    const Pixel* screen = a_pixels + y * a_width + x;

    for(int i = h; i--; ) {
        memcpy(dst, screen, w * sizeof(Pixel));
        dst += w;
        screen += a_width;
    }
}

void a_screen_setTarget(Pixel* const p, const int w, const int h)
{
    a_pixels = p;
    a_width = w;
    a_height = h;
}

void a_screen_setTargetSprite(const Sprite* const s)
{
    a_pixels = s->data;
    a_width = s->w;
    a_height = s->h;
}

void a_screen_resetTarget(void)
{
    a_pixels = a__pixels2;
    a_width = a__width2;
    a_height = a__height2;
}

#if A_PLATFORM_LINUXPC || A_PLATFORM_WINDOWS
    void a_screen__full(void)
    {
        videoFlags ^= SDL_FULLSCREEN;
        setSDLScreen();
    }

    void a_screen__double(void)
    {
        // once this function is called, we switch to fake screen permanently
        if(!a2x_bool("video.fake")) {
            setFakeScreen();
            a2x__set("video.fake", "1");
        }

        setSDLScreen();
    }
#endif

static void setSDLScreen(void)
{
    if(a2x_bool("video.double")) {
        a_screen = SDL_SetVideoMode(a_width * 2, a_height * 2, A_BPP, videoFlags);
    } else {
        a_screen = SDL_SetVideoMode(a_width, a_height, A_BPP, videoFlags);
    }

    SDL_SetClipRect(a_screen, NULL);

    if(!a2x_bool("video.fake")) {
        if(SDL_MUSTLOCK(a_screen)) {
            SDL_LockSurface(a_screen);
        }

        a_pixels = a_screen->pixels;
        a__pixels2 = a_pixels;
    }
}

static void setFakeScreen(void)
{
    a_pixels = malloc(A_SCREEN_SIZE);
    memset(a_pixels, 0, A_SCREEN_SIZE);

    a__pixels2 = a_pixels;
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
