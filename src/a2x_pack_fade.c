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

#define pixel_red1(p)   ((((p) >> (11 + 16)) & a_mask(5)) << 3)
#define pixel_green1(p) ((((p) >> (5  + 16)) & a_mask(6)) << 2)
#define pixel_blue1(p)  ((((p) >> (0  + 16)) & a_mask(5)) << 3)

#define pixel_red2(p)   ((((p) >> (11 + 0 )) & a_mask(5)) << 3)
#define pixel_green2(p) ((((p) >> (5  + 0 )) & a_mask(6)) << 2)
#define pixel_blue2(p)  ((((p) >> (0  + 0 )) & a_mask(5)) << 3)

void a_fade_toBlack(int framesDuration)
{
    int accum = 0;
    fix alpha = A_FIX_ONE;
    Pixel* const copy = a_screen_dup();

    while(alpha >= 0) {
        a_fps_start();

        a_screen_copy(a_pixels, copy);
        uint32_t* a_pixels2 = (uint32_t*)a_pixels;

        for(int i = SCREEN_DIM / 2; i--; ) {
            const uint32_t c = *a_pixels2;

            *a_pixels2++ = (a_pixel_make(
                a_fix_fixtoi(pixel_red1(c)   * alpha),
                a_fix_fixtoi(pixel_green1(c) * alpha),
                a_fix_fixtoi(pixel_blue1(c)  * alpha)
            ) << 16) | a_pixel_make(
                a_fix_fixtoi(pixel_red2(c)   * alpha),
                a_fix_fixtoi(pixel_green2(c) * alpha),
                a_fix_fixtoi(pixel_blue2(c)  * alpha)
            );
        }

        if(framesDuration < A_FIX_ONE) {
            while(accum < A_FIX_ONE) {
                accum += framesDuration;
                alpha--;
            }

            accum -= A_FIX_ONE;
        } else if(A_FIX_ONE <= framesDuration) {
            accum += A_FIX_ONE;

            if(accum >= framesDuration) {
                accum -= framesDuration;
                alpha--;
            }
        }

        a_fps_end();
    }

    free(copy);
}

void a_fade_fromBlack(int framesDuration)
{
    int accum = 0;
    fix alpha = 0;
    Pixel* const copy = a_screen_dup();

    while(alpha <= A_FIX_ONE) {
        a_fps_start();

        a_screen_copy(a_pixels, copy);
        uint32_t* a_pixels2 = (uint32_t*)a_pixels;

        for(int i = SCREEN_DIM / 2; i--; ) {
            const uint32_t c = *a_pixels2;

            *a_pixels2++ = (a_pixel_make(
                a_fix_fixtoi(pixel_red1(c)   * alpha),
                a_fix_fixtoi(pixel_green1(c) * alpha),
                a_fix_fixtoi(pixel_blue1(c)  * alpha)
            ) << 16) | a_pixel_make(
                a_fix_fixtoi(pixel_red2(c)   * alpha),
                a_fix_fixtoi(pixel_green2(c) * alpha),
                a_fix_fixtoi(pixel_blue2(c)  * alpha)
            );
        }

        if(framesDuration < A_FIX_ONE) {
            while(accum < A_FIX_ONE) {
                accum += framesDuration;
                alpha++;
            }

            accum -= A_FIX_ONE;
        } else if(A_FIX_ONE <= framesDuration) {
            accum += A_FIX_ONE;

            if(accum >= framesDuration) {
                accum -= framesDuration;
                alpha++;
            }
        }

        a_fps_end();
    }

    free(copy);
}

void a_fade_screens(const Pixel* const old, int framesDuration)
{
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

    int accum = 0;
    fix alpha = A_FIX_ONE;

    while(alpha >= 0) {
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
                *rNewp + a_fix_fixtoi(*rOldp * alpha),
                *gNewp + a_fix_fixtoi(*gOldp * alpha),
                *bNewp + a_fix_fixtoi(*bOldp * alpha)
            );

            rNewp++;
            gNewp++;
            bNewp++;

            rOldp++;
            gOldp++;
            bOldp++;
        }

        if(framesDuration < A_FIX_ONE) {
            while(accum < A_FIX_ONE) {
                accum += framesDuration;
                alpha--;
            }

            accum -= A_FIX_ONE;
        } else if(A_FIX_ONE <= framesDuration) {
            accum += A_FIX_ONE;

            if(accum >= framesDuration) {
                accum -= framesDuration;
                alpha--;
            }
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
