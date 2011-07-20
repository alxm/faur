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

#include "a2x_pack_fade.v.h"

#define SCREEN_DIM (a_width * a_height)

#define setSpeed() (speed = FONE8 >> (speed + 1 + a2x_int("fps") / 60))

#define pixel_red1(p)   ((((p) >> (11 + 16)) & a_mask(5)) << 3)
#define pixel_green1(p) ((((p) >> (5  + 16)) & a_mask(6)) << 2)
#define pixel_blue1(p)  ((((p) >> (0  + 16)) & a_mask(5)) << 3)

#define pixel_red2(p)   ((((p) >> (11 + 0 )) & a_mask(5)) << 3)
#define pixel_green2(p) ((((p) >> (5  + 0 )) & a_mask(6)) << 2)
#define pixel_blue2(p)  ((((p) >> (0  + 0 )) & a_mask(5)) << 3)

void a_fade_toBlack(FadeSpeed speed)
{
    setSpeed();

    Pixel* const copy = a_screen_dup();

    for(fix8 a = FONE8; a >= 0; a -= speed) {
        a_fps_start();

        a_screen_copy(a_pixels, copy);
        uint32_t* a_pixels2 = (uint32_t*)a_pixels;

        for(int i = SCREEN_DIM / 2; i--; ) {
            const uint32_t c = *a_pixels2;

            *a_pixels2++ = (a_pixel_make(
                a_fix8_fixtoi(pixel_red1(c)   * a),
                a_fix8_fixtoi(pixel_green1(c) * a),
                a_fix8_fixtoi(pixel_blue1(c)  * a)
            ) << 16) | a_pixel_make(
                a_fix8_fixtoi(pixel_red2(c)   * a),
                a_fix8_fixtoi(pixel_green2(c) * a),
                a_fix8_fixtoi(pixel_blue2(c)  * a)
            );
        }

        a_fps_end();
    }

    free(copy);
}

void a_fade_fromBlack(FadeSpeed speed)
{
    setSpeed();

    Pixel* const copy = a_screen_dup();

    for(fix8 a = 0; a <= FONE8; a += speed) {
        a_fps_start();

        a_screen_copy(a_pixels, copy);
        uint32_t* a_pixels2 = (uint32_t*)a_pixels;

        for(int i = SCREEN_DIM / 2; i--; ) {
            const uint32_t c = *a_pixels2;

            *a_pixels2++ = (a_pixel_make(
                a_fix8_fixtoi(pixel_red1(c)   * a),
                a_fix8_fixtoi(pixel_green1(c) * a),
                a_fix8_fixtoi(pixel_blue1(c)  * a)
            ) << 16) | a_pixel_make(
                a_fix8_fixtoi(pixel_red2(c)   * a),
                a_fix8_fixtoi(pixel_green2(c) * a),
                a_fix8_fixtoi(pixel_blue2(c)  * a)
            );
        }

        a_fps_end();
    }

    free(copy);
}

void a_fade_screens(const Pixel* const old, FadeSpeed speed)
{
    setSpeed();

    int* rNew = malloc(SCREEN_DIM * sizeof(int));
    int* gNew = malloc(SCREEN_DIM * sizeof(int));
    int* bNew = malloc(SCREEN_DIM * sizeof(int));

    int* rOld = malloc(SCREEN_DIM * sizeof(int));
    int* gOld = malloc(SCREEN_DIM * sizeof(int));
    int* bOld = malloc(SCREEN_DIM * sizeof(int));

    int* rNewp = rNew;
    int* gNewp = gNew;
    int* bNewp = bNew;

    int* rOldp = rOld;
    int* gOldp = gOld;
    int* bOldp = bOld;

    Pixel* newp = a_pixels;
    Pixel* oldp = (Pixel*)old;

    for(int i = SCREEN_DIM; i--; ) {
        const Pixel nc = *newp++;
        const Pixel oc = *oldp++;

        *rNewp = a_pixel_red(nc);
        *rOldp++ = a_pixel_red(oc) - *rNewp++;

        *gNewp = a_pixel_green(nc);
        *gOldp++ = a_pixel_green(oc) - *gNewp++;

        *bNewp = a_pixel_blue(nc);
        *bOldp++ = a_pixel_blue(oc) - *bNewp++;
    }

    for(fix8 a = FONE8; a >= 0; a -= speed) {
        a_fps_start();

        Pixel* dst = a_pixels;

        rNewp = rNew;
        gNewp = gNew;
        bNewp = bNew;

        rOldp = rOld;
        gOldp = gOld;
        bOldp = bOld;

        for(int i = SCREEN_DIM; i--; ) {
            *dst++ = a_pixel_make(
                *rNewp + a_fix8_fixtoi(*rOldp * a),
                *gNewp + a_fix8_fixtoi(*gOldp * a),
                *bNewp + a_fix8_fixtoi(*bOldp * a)
            );

            rNewp++;
            gNewp++;
            bNewp++;

            rOldp++;
            gOldp++;
            bOldp++;
        }

        a_fps_end();
    }

    free(rNew);
    free(gNew);
    free(bNew);

    free(rOld);
    free(gOld);
    free(bOld);
}
