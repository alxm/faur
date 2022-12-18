/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

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

#include "f_fade.v.h"
#include <faur.v.h>

typedef enum {
    F__FADE_INVALID = -1,
    F__FADE_TOCOLOR,
    F__FADE_FROMCOLOR,
    F__FADE_SCREENS,
    F__FADE_CUSTOM,
    F__FADE_NUM
} FFadeOpId;

static struct {
    FEvent event;
    FFadeOpId op;
    FFixu angle, angleInc;
    FSprite* oldScreen;
    union {
        FColorPixel color;
        FCallFade* callback;
    } u;
} g_fade = {
    .op = F__FADE_INVALID,
};

static void f_fade__init(void)
{
    #if !F_CONFIG_TRAIT_LOW_MEM
        g_fade.oldScreen = f_sprite_newBlank(f__screen.pixels->size.x,
                                             f__screen.pixels->size.y,
                                             1,
                                             false);
    #endif
}

static void f_fade__uninit(void)
{
    f_sprite_free(g_fade.oldScreen);
}

const FPack f_pack__fade = {
    "Fade",
    f_fade__init,
    f_fade__uninit,
};

const FEvent* f_fade_eventGet(void)
{
    return &g_fade.event;
}

static void newFade(FFadeOpId Op, unsigned DurationMs)
{
    g_fade.event = 1;
    g_fade.op = Op;
    g_fade.angle = 0;

    g_fade.angleInc =
        DurationMs == 0
            ? F_DEG_090_FIX
            : f_fixu_div(F_DEG_090_FIX, f_time_msToTicks(DurationMs));
}

void f_fade_startColorTo(unsigned DurationMs)
{
    newFade(F__FADE_TOCOLOR, DurationMs);

    g_fade.u.color = f__color.pixel;
}

void f_fade_startColorFrom(unsigned DurationMs)
{
    newFade(F__FADE_FROMCOLOR, DurationMs);

    g_fade.u.color = f__color.pixel;
}

#if !F_CONFIG_TRAIT_LOW_MEM
void f_fade_startScreens(unsigned DurationMs)
{
    newFade(F__FADE_SCREENS, DurationMs);

    f_screen__toSprite(g_fade.oldScreen, 0);
}
#endif

void f_fade_startCustom(FCallFade* Callback, unsigned DurationMs)
{
    F__CHECK(Callback != NULL);

    newFade(F__FADE_CUSTOM, DurationMs);

    g_fade.u.callback = Callback;
}

void f_fade__tick(void)
{
    if(g_fade.op == F__FADE_INVALID) {
        return;
    }

    g_fade.angle += g_fade.angleInc;

    if(g_fade.angle > F_DEG_090_FIX) {
        g_fade.event = 0;
        g_fade.op = F__FADE_INVALID;
    }
}

void f_fade__draw(void)
{
    if(g_fade.op == F__FADE_INVALID) {
        return;
    }

    f_color_push();

    switch(g_fade.op) {
        case F__FADE_TOCOLOR: {
            f_color_blendSet(F_COLOR_BLEND_ALPHA);
            f_color_alphaSet(
                f_fix_toInt(f_fix_sinf(g_fade.angle) * F_COLOR_ALPHA_MAX));

            f_color_colorSetPixel(g_fade.u.color);
            f_draw_fill();
        } break;

        case F__FADE_FROMCOLOR: {
            f_color_blendSet(F_COLOR_BLEND_ALPHA);
            f_color_alphaSet(
                f_fix_toInt(f_fix_sinf(F_DEG_090_FIX - g_fade.angle)
                                * F_COLOR_ALPHA_MAX));

            f_color_colorSetPixel(g_fade.u.color);
            f_draw_fill();
        } break;

        case F__FADE_SCREENS: {
            f_color_blendSet(F_COLOR_BLEND_ALPHA);
            f_color_alphaSet(
                f_fix_toInt(f_fix_sinf(F_DEG_090_FIX - g_fade.angle)
                                * F_COLOR_ALPHA_MAX));

            f_sprite_blit(g_fade.oldScreen, 0, 0, 0);
        } break;

        case F__FADE_CUSTOM: {
            g_fade.u.callback(f_fix_sinf(g_fade.angle));
        } break;

        default: break;
    }

    f_color_pop();
}
