/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_fade.v.h"
#include <a2x.v.h>

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
    ASprite* oldScreen;
} g_fade = {
    .op = A__FADE_INVALID,
};

static void a_fade__init(void)
{
    #if !A_CONFIG_SYSTEM_GAMEBUINO
        g_fade.oldScreen = a_sprite_newBlank(
                            a__screen.pixels->w, a__screen.pixels->h, 1, false);
    #endif
}

static void a_fade__uninit(void)
{
    a_sprite_free(g_fade.oldScreen);
}

const APack a_pack__fade = {
    "Fade",
    {
        [0] = a_fade__init,
    },
    {
        [0] = a_fade__uninit,
    },
};

const AEvent* a_fade_eventGet(void)
{
    return &g_fade.event;
}

static void newFade(AFadeOpId Op, unsigned DurationMs)
{
    g_fade.event = 1;
    g_fade.op = Op;
    g_fade.angle = 0;
    g_fade.angleInc = A_DEG_090_FIX
                        / a_math_maxu(a_time_ticksFromMs(DurationMs), 1);
}

void a_fade_startColorTo(unsigned DurationMs)
{
    newFade(A__FADE_TOCOLOR, DurationMs);

    g_fade.color = a__color.pixel;
}

void a_fade_startColorFrom(unsigned DurationMs)
{
    newFade(A__FADE_FROMCOLOR, DurationMs);

    g_fade.color = a__color.pixel;
}

#if !A_CONFIG_SYSTEM_GAMEBUINO
void a_fade_startScreens(unsigned DurationMs)
{
    newFade(A__FADE_SCREENS, DurationMs);

    a_screen__toSprite(g_fade.oldScreen, 0);
}
#endif

void a_fade__tick(void)
{
    if(g_fade.op == A__FADE_INVALID) {
        return;
    }

    g_fade.angle += g_fade.angleInc;

    if(g_fade.angle > A_DEG_090_FIX) {
        g_fade.event = 0;
        g_fade.op = A__FADE_INVALID;
    }
}

void a_fade__draw(void)
{
    if(g_fade.op == A__FADE_INVALID) {
        return;
    }

    a_color_push();
    a_color_blendSet(A_COLOR_BLEND_RGBA);

    switch(g_fade.op) {
        case A__FADE_TOCOLOR: {
            a_color_alphaSet(
                a_fix_toInt(a_fix_sinf(g_fade.angle) * A_COLOR_ALPHA_MAX));

            a_color_baseSetPixel(g_fade.color);
            a_draw_fill();
        } break;

        case A__FADE_FROMCOLOR: {
            a_color_alphaSet(
                a_fix_toInt(a_fix_sinf(A_DEG_090_FIX - g_fade.angle)
                                * A_COLOR_ALPHA_MAX));

            a_color_baseSetPixel(g_fade.color);
            a_draw_fill();
        } break;

        case A__FADE_SCREENS: {
            a_color_alphaSet(
                a_fix_toInt(a_fix_sinf(A_DEG_090_FIX - g_fade.angle)
                                * A_COLOR_ALPHA_MAX));

            a_sprite_blit(g_fade.oldScreen, 0, 0, 0);
        } break;

        default: break;
    }

    a_color_pop();
}
