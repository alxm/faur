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

#ifndef A2X_PACK_SCREEN_PH
#define A2X_PACK_SCREEN_PH

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <SDL/SDL.h>

typedef uint16_t Pixel;

#include "a2x_app_settings.p.h"

#include "a2x_pack_draw.p.h"
#include "a2x_pack_sound.p.h"
#include "a2x_pack_str.p.h"
#include "a2x_pack_time.p.h"

extern void a__screen_set(void);
extern void a__screen_free(void);

#define A_BPP 16

#define a_mask(x) ((1 << (x)) - 1)

// RGB565
#define a_screen_makePixel(r, g, b) \
(                                   \
    (                               \
        (((r) >> 3) << 11)          \
      | (((g) >> 2) << 5 )          \
      | (((b) >> 3) << 0 )          \
    ) & a_mask(16)                  \
)

#define a_screen_pixelRed(p)   ((((p) >> 11) & a_mask(5)) << 3)
#define a_screen_pixelGreen(p) ((((p) >> 5 ) & a_mask(6)) << 2)
#define a_screen_pixelBlue(p)  ((((p) >> 0 ) & a_mask(5)) << 3)

#define a_screen_rgbToRaw(r, g, b) (((r) << 24) | ((g) << 16) | ((b) << 8))
#define a_screen_rawRed(r)         (((r) >> 24) & 0xff)
#define a_screen_rawGreen(r)       (((r) >> 16) & 0xff)
#define a_screen_rawBlue(r)        (((r) >> 8 ) & 0xff)

#define a_screen_getPixel(x, y) (*(a_pixels + (y) * a_width + (x)))
#define a_screen_getPixel2(p, x, y) (*((p) + (y) * a_width + (x)))



extern SDL_Surface* a_screen;
extern Pixel* a_pixels;
extern int a_width;
extern int a_height;

#define A_SCREEN_SIZE (a2xSet.width * a2xSet.height * sizeof(Pixel))

#define a_screen_new()          malloc(A_SCREEN_SIZE)
#define a_screen_copy(dst, src) memcpy((dst), (src), A_SCREEN_SIZE)

#define a_screen_dup(src)                 \
({                                        \
    Pixel* const a__dst = a_screen_new(); \
    a_screen_copy(a__dst, (src));         \
    a__dst;                               \
})

extern void a_screen_setTarget(Pixel* const p, const int w, const int h);
extern void a_screen_resetTarget(void);

extern void a__screen_switchFull(void);

extern void a_screen_show(void);
extern void a_screen_custom(void (*f)(void* const v), void* const v);

#endif // A2X_PACK_SCREEN_PH
