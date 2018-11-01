/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_draw.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_time.v.h"

typedef enum {
    A__FADE_INVALID = -1,
    A__FADE_TOCOLOR,
    A__FADE_FROMCOLOR,
    A__FADE_SCREENS,
    A__FADE_NUM
} AFadeOpId;

static struct {
    AEvent event;
    AFadeOpId op;
    AFixu angle, angleInc;
    APixel color;
    AScreen* capturedScreen;
} g_fade = {
    .op = A__FADE_INVALID,
};

void a_fade__init(void)
{
    g_fade.capturedScreen = a_screen_new(a__screen.width, a__screen.height);
}

void a_fade__uninit(void)
{
    a_screen_free(g_fade.capturedScreen);
}

AEvent* a_fade_eventGet(void)
{
    return &g_fade.event;
}

static void newFade(AFadeOpId Op, unsigned DurationMs)
{
    g_fade.event = 1;
    g_fade.op = Op;
    g_fade.angle = 0;
    g_fade.angleInc = A_FIX_DEG_090 / a_time_msToTicks(DurationMs);
}

void a_fade_toColor(unsigned DurationMs)
{
    newFade(A__FADE_TOCOLOR, DurationMs);

    g_fade.color = a_pixel__state.pixel;
}

void a_fade_fromColor(unsigned DurationMs)
{
    newFade(A__FADE_FROMCOLOR, DurationMs);

    g_fade.color = a_pixel__state.pixel;
}

void a_fade_screens(unsigned DurationMs)
{
    newFade(A__FADE_SCREENS, DurationMs);

    a_screen_copy(g_fade.capturedScreen, &a__screen);
}

void a_fade__tick(void)
{
    if(g_fade.op == A__FADE_INVALID) {
        return;
    }

    g_fade.angle += g_fade.angleInc;

    if(g_fade.angle >= A_FIX_DEG_090) {
        g_fade.event = 0;
        g_fade.op = A__FADE_INVALID;
    }
}

void a_fade__draw(void)
{
    if(g_fade.op == A__FADE_INVALID) {
        return;
    }

    a_pixel_push();
    a_pixel_blendSet(A_PIXEL_BLEND_RGBA);

    switch(g_fade.op) {
        case A__FADE_TOCOLOR: {
            a_pixel_alphaSet(
                a_fix_toInt(a_fix_sinf(g_fade.angle) * A_PIXEL_ALPHA_MAX));

            a_pixel_colorSetPixel(g_fade.color);
            a_draw_fill();
        } break;

        case A__FADE_FROMCOLOR: {
            a_pixel_alphaSet(
                a_fix_toInt(a_fix_sinf(A_FIX_DEG_090 - g_fade.angle)
                                * A_PIXEL_ALPHA_MAX));

            a_pixel_colorSetPixel(g_fade.color);
            a_draw_fill();
        } break;

        case A__FADE_SCREENS: {
            a_pixel_alphaSet(
                a_fix_toInt(a_fix_sinf(A_FIX_DEG_090 - g_fade.angle)
                                * A_PIXEL_ALPHA_MAX));

            a_screen_blit(g_fade.capturedScreen);
        } break;

        default: break;
    }

    a_pixel_pop();
}
