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

static int g_framesDuration;
static const APixel* g_oldScreen;

static A_STATE(a_fade__toBlack);
static A_STATE(a_fade__fromBlack);
static A_STATE(a_fade__screens);

void a_fade__init(void)
{
    a_state_new("a__fadeToBlack", a_fade__toBlack);
    a_state_new("a__fadeFromBlack", a_fade__fromBlack);
    a_state_new("a__fadeScreens", a_fade__screens);
}

void a_fade_toBlack(int FramesDuration)
{
    g_framesDuration = FramesDuration;
    a_state_push("a__fadeToBlack");
}

void a_fade_fromBlack(int FramesDuration)
{
    g_framesDuration = FramesDuration;
    a_state_push("a__fadeFromBlack");
}

void a_fade_screens(const APixel* OldScreen, int FramesDuration)
{
    g_oldScreen = OldScreen;
    g_framesDuration = FramesDuration;
    a_state_push("a__fadeScreens");
}

#define SCREEN_DIM (a_screen__width * a_screen__height)

static A_STATE(a_fade__toBlack)
{
    A_STATE_BODY
    {
        AFix alpha = 0;
        AFix alpha_inc = a_fix_itofix(255) / g_framesDuration;
        APixel* const copy = a_screen_dup();

        a_pixel_setBlend(A_PIXEL_RGBA);
        a_pixel_setRGB(0, 0, 0);

        A_STATE_LOOP
        {
            a_screen_copy(a_screen__pixels, copy);

            a_pixel_setAlpha(a_fix_fixtoi(alpha));
            a_draw_fill();

            alpha += alpha_inc;

            if(alpha > a_fix_itofix(255)) {
                a_state_pop();
            }
        }

        free(copy);
    }
}

static A_STATE(a_fade__fromBlack)
{
    A_STATE_BODY
    {
        AFix alpha = a_fix_itofix(255);
        AFix alpha_inc = a_fix_itofix(255) / g_framesDuration;
        APixel* const copy = a_screen_dup();

        a_pixel_setBlend(A_PIXEL_RGBA);
        a_pixel_setRGB(0, 0, 0);

        A_STATE_LOOP
        {
            a_screen_copy(a_screen__pixels, copy);

            a_pixel_setAlpha(a_fix_fixtoi(alpha));
            a_draw_fill();

            alpha -= alpha_inc;

            if(alpha < 0) {
                a_state_pop();
            }
        }

        free(copy);
    }
}

static A_STATE(a_fade__screens)
{
    A_STATE_BODY
    {
        int* rNew = a_mem_malloc(SCREEN_DIM * sizeof(int));
        int* gNew = a_mem_malloc(SCREEN_DIM * sizeof(int));
        int* bNew = a_mem_malloc(SCREEN_DIM * sizeof(int));

        int* rOld = a_mem_malloc(SCREEN_DIM * sizeof(int));
        int* gOld = a_mem_malloc(SCREEN_DIM * sizeof(int));
        int* bOld = a_mem_malloc(SCREEN_DIM * sizeof(int));

        int* rNewp = rNew;
        int* gNewp = gNew;
        int* bNewp = bNew;

        int* rOldp = rOld;
        int* gOldp = gOld;
        int* bOldp = bOld;

        APixel* newp = a_screen__pixels;
        APixel* oldp = (APixel*)g_oldScreen;

        for(int i = SCREEN_DIM; i--; ) {
            const APixel nc = *newp++;
            const APixel oc = *oldp++;

            *rNewp = a_pixel_red(nc);
            *rOldp++ = a_pixel_red(oc) - *rNewp++;

            *gNewp = a_pixel_green(nc);
            *gOldp++ = a_pixel_green(oc) - *gNewp++;

            *bNewp = a_pixel_blue(nc);
            *bOldp++ = a_pixel_blue(oc) - *bNewp++;
        }

        int accum = 0;
        AFix alpha = A_FIX_ONE;

        A_STATE_LOOP
        {
            APixel* dst = a_screen__pixels;

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

            if(g_framesDuration < A_FIX_ONE) {
                while(accum < A_FIX_ONE) {
                    accum += g_framesDuration;
                    alpha--;
                }

                accum -= A_FIX_ONE;
            } else if(A_FIX_ONE <= g_framesDuration) {
                accum += A_FIX_ONE;

                if(accum >= g_framesDuration) {
                    accum -= g_framesDuration;
                    alpha--;
                }
            }

            if(alpha < 0) {
                a_state_pop();
            }
        }

        free(rNew);
        free(gNew);
        free(bNew);

        free(rOld);
        free(gOld);
        free(bOld);
    }
}
