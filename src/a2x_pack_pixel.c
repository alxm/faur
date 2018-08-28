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

#include "a2x_pack_pixel.v.h"

#include "a2x_pack_list.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_platform_software_blit.v.h"
#include "a2x_pack_platform_software_draw.v.h"

APixelState a_pixel__state;
static AList* g_stateStack;

void a_pixel__init(void)
{
    g_stateStack = a_list_new();
    a_pixel_reset();
}

void a_pixel__uninit(void)
{
    a_list_freeEx(g_stateStack, free);
}

void a_pixel_push(void)
{
    a_list_push(g_stateStack, a_mem_dup(&a_pixel__state, sizeof(APixelState)));
}

void a_pixel_pop(void)
{
    APixelState* state = a_list_pop(g_stateStack);

    if(state == NULL) {
        a_out__fatal("Cannot pop APixelState: stack is empty");
    }

    a_pixel__state = *state;
    free(state);

    a_pixel_blendSet(a_pixel__state.blend);
    a_pixel_colorSetRgba(a_pixel__state.red,
                         a_pixel__state.green,
                         a_pixel__state.blue,
                         a_pixel__state.alpha);
}

void a_pixel_reset(void)
{
    a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);
    a_pixel_colorSetRgba(0, 0, 0, A_PIXEL_ALPHA_MAX);
    a_pixel_fillBlitSet(false);
    a_pixel_fillDrawSet(true);
}

#if A_PLATFORM_RENDER_SOFTWARE
static void optimizeAlphaBlending(bool UpdateRoutines)
{
    if(a_pixel__state.canonicalBlend == A_PIXEL_BLEND_RGBA) {
        APixelBlend fastestBlend = A_PIXEL_BLEND_RGBA;

        switch(a_pixel__state.alpha) {
            case A_PIXEL_ALPHA_MAX / 4: {
                fastestBlend = A_PIXEL_BLEND_RGB25;
            } break;

            case A_PIXEL_ALPHA_MAX / 2: {
                fastestBlend = A_PIXEL_BLEND_RGB50;
            } break;

            case A_PIXEL_ALPHA_MAX * 3 / 4: {
                fastestBlend = A_PIXEL_BLEND_RGB75;
            } break;

            case A_PIXEL_ALPHA_MAX: {
                fastestBlend = A_PIXEL_BLEND_PLAIN;
            } break;
        }

        if(a_pixel__state.blend != fastestBlend) {
            a_pixel__state.blend = fastestBlend;

            if(UpdateRoutines) {
                a_platform_software_draw__updateRoutines();
                a_platform_software_blit__updateRoutines();
            }
        }
    }
}
#endif

APixelBlend a_pixel_blendGet(void)
{
    return a_pixel__state.blend;
}

void a_pixel_blendSet(APixelBlend Blend)
{
    a_pixel__state.blend = Blend;
    a_pixel__state.canonicalBlend = Blend;

    #if A_PLATFORM_RENDER_SOFTWARE
        optimizeAlphaBlending(false);
        a_platform_software_draw__updateRoutines();
        a_platform_software_blit__updateRoutines();
    #else
        if(Blend == A_PIXEL_BLEND_RGB25) {
            a_pixel_alphaSet(A_PIXEL_ALPHA_MAX / 4);
        } else if(Blend == A_PIXEL_BLEND_RGB50) {
            a_pixel_alphaSet(A_PIXEL_ALPHA_MAX / 2);
        } else if(Blend == A_PIXEL_BLEND_RGB75) {
            a_pixel_alphaSet(A_PIXEL_ALPHA_MAX * 3 / 4);
        }

        a_platform__renderSetBlendMode();
    #endif
}

int a_pixel_alphaGet(void)
{
    return a_pixel__state.alpha;
}

void a_pixel_alphaSet(int Alpha)
{
    a_pixel__state.alpha = a_math_clamp(Alpha, 0, A_PIXEL_ALPHA_MAX);

    #if A_PLATFORM_RENDER_SOFTWARE
        optimizeAlphaBlending(true);
    #else
        a_platform__renderSetDrawColor();
    #endif
}

void a_pixel_colorSetRgb(int Red, int Green, int Blue)
{
    a_pixel__state.red = (unsigned)Red & 0xff;
    a_pixel__state.green = (unsigned)Green & 0xff;
    a_pixel__state.blue = (unsigned)Blue & 0xff;
    a_pixel__state.pixel = a_pixel_fromRgb(Red, Green, Blue);

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__renderSetDrawColor();
    #endif
}

void a_pixel_colorSetRgba(int Red, int Green, int Blue, int Alpha)
{
    a_pixel__state.red = (unsigned)Red & 0xff;
    a_pixel__state.green = (unsigned)Green & 0xff;
    a_pixel__state.blue = (unsigned)Blue & 0xff;
    a_pixel__state.alpha = a_math_clamp(Alpha, 0, A_PIXEL_ALPHA_MAX);
    a_pixel__state.pixel = a_pixel_fromRgb(Red, Green, Blue);

    #if A_PLATFORM_RENDER_SOFTWARE
        optimizeAlphaBlending(true);
    #else
        a_platform__renderSetDrawColor();
    #endif
}

void a_pixel_colorSetHex(uint32_t Hexcode)
{
    a_pixel__state.red = (Hexcode >> 16) & 0xff;
    a_pixel__state.green = (Hexcode >> 8) & 0xff;
    a_pixel__state.blue = Hexcode & 0xff;
    a_pixel__state.pixel = a_pixel_fromHex(Hexcode);

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__renderSetDrawColor();
    #endif
}

void a_pixel_colorSetPixel(APixel Pixel)
{
    a_pixel_toRgb(Pixel,
                  &a_pixel__state.red,
                  &a_pixel__state.green,
                  &a_pixel__state.blue);

    a_pixel__state.pixel = Pixel;

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__renderSetDrawColor();
    #endif
}

void a_pixel_fillBlitSet(bool Fill)
{
    a_pixel__state.fillBlit = Fill;

    #if A_PLATFORM_RENDER_SOFTWARE
        a_platform_software_blit__updateRoutines();
    #endif
}

void a_pixel_fillDrawSet(bool Fill)
{
    a_pixel__state.fillDraw = Fill;

    #if A_PLATFORM_RENDER_SOFTWARE
        a_platform_software_draw__updateRoutines();
    #endif
}
