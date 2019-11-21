/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_color.v.h"
#include <faur.v.h>

FPixelState f__color;
static FList* g_stateStack;

FColorPixel f_color__key;
FColorPixel f_color__limit;

static void f_color__init(void)
{
    g_stateStack = f_list_new();
    f_color_reset();

    f_color__key = f_color_pixelFromHex(F_CONFIG_COLOR_SPRITE_KEY);
    f_color__limit = f_color_pixelFromHex(F_CONFIG_COLOR_SPRITE_BORDER);
}

static void f_color__uninit(void)
{
    f_list_freeEx(g_stateStack, f_mem_free);
}

const FPack f_pack__color = {
    "Color",
    {
        [0] = f_color__init,
    },
    {
        [0] = f_color__uninit,
    },
};

void f_color_push(void)
{
    f_list_push(g_stateStack, f_mem_dup(&f__color, sizeof(FPixelState)));
}

void f_color_pop(void)
{
    FPixelState* state = f_list_pop(g_stateStack);

    #if F_CONFIG_BUILD_DEBUG
        if(state == NULL) {
            F__FATAL("f_color_pop: Stack is empty");
        }
    #endif

    f__color = *state;
    f_mem_free(state);

    f_color_blendSet(f__color.blend);
    f_color_colorSetRgba(
        f__color.rgb.r, f__color.rgb.g, f__color.rgb.b, f__color.alpha);
}

void f_color_reset(void)
{
    f_color_blendSet(F_COLOR_BLEND_PLAIN);
    f_color_colorSetRgba(0, 0, 0, F_COLOR_ALPHA_MAX);
    f_color_fillBlitSet(false);
    f_color_fillDrawSet(true);
}

#if F__OPTIMIZE_ALPHA
static void optimizeAlphaBlending(void)
{
    if(f__color.canonicalBlend == F_COLOR_BLEND_ALPHA) {
        FColorBlend fastestBlend = F_COLOR_BLEND_ALPHA;

        switch(f__color.alpha) {
            case F_COLOR_ALPHA_MAX / 4: {
                fastestBlend = F_COLOR_BLEND_ALPHA_25;
            } break;

            case F_COLOR_ALPHA_MAX / 2: {
                fastestBlend = F_COLOR_BLEND_ALPHA_50;
            } break;

            case F_COLOR_ALPHA_MAX * 3 / 4: {
                fastestBlend = F_COLOR_BLEND_ALPHA_75;
            } break;

            case F_COLOR_ALPHA_MAX: {
                fastestBlend = F_COLOR_BLEND_PLAIN;
            } break;
        }

        if(f__color.blend != fastestBlend) {
            f__color.blend = fastestBlend;
        }
    }
}
#endif

void f_color_blendSet(FColorBlend Blend)
{
    f__color.blend = Blend;

    #if F__OPTIMIZE_ALPHA
        f__color.canonicalBlend = Blend;
        optimizeAlphaBlending();
    #else
        if(Blend == F_COLOR_BLEND_ALPHA_25) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX / 4);
        } else if(Blend == F_COLOR_BLEND_ALPHA_50) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX / 2);
        } else if(Blend == F_COLOR_BLEND_ALPHA_75) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX * 3 / 4);
        }

        #if !F_CONFIG_RENDER_SOFTWARE
            f_platform_api__renderSetBlendMode();
        #endif
    #endif
}

void f_color_alphaSet(int Alpha)
{
    f__color.alpha = f_math_clamp(Alpha, 0, F_COLOR_ALPHA_MAX);

    #if F__OPTIMIZE_ALPHA
        optimizeAlphaBlending();
    #elif !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__renderSetDrawColor();
    #endif
}

static void setRgb(int Red, int Green, int Blue)
{
    f__color.rgb.r = (unsigned)Red & 0xff;
    f__color.rgb.g = (unsigned)Green & 0xff;
    f__color.rgb.b = (unsigned)Blue & 0xff;

    f__color.pixel = f_color_pixelFromRgb3(Red, Green, Blue);
}

void f_color_colorSetRgb(int Red, int Green, int Blue)
{
    setRgb(Red, Green, Blue);

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__renderSetDrawColor();
    #endif
}

void f_color_colorSetRgba(int Red, int Green, int Blue, int Alpha)
{
    setRgb(Red, Green, Blue);
    f__color.alpha = f_math_clamp(Alpha, 0, F_COLOR_ALPHA_MAX);

    #if F__OPTIMIZE_ALPHA
        optimizeAlphaBlending();
    #elif !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__renderSetDrawColor();
    #endif
}

void f_color_colorSetHex(uint32_t Hexcode)
{
    f__color.rgb.r = (Hexcode >> 16) & 0xff;
    f__color.rgb.g = (Hexcode >> 8)  & 0xff;
    f__color.rgb.b = (Hexcode)       & 0xff;
    f__color.pixel = f_color_pixelFromHex(Hexcode);

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__renderSetDrawColor();
    #endif
}

void f_color_colorSetPixel(FColorPixel Pixel)
{
    f__color.rgb = f_color_pixelToRgb(Pixel);
    f__color.pixel = Pixel;

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__renderSetDrawColor();
    #endif
}

void f_color_fillBlitSet(bool Fill)
{
    f__color.fillBlit = Fill;
}

void f_color_fillDrawSet(bool Fill)
{
    f__color.fillDraw = Fill;
}
