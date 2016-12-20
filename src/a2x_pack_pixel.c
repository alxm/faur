/*
    Copyright 2010, 2016 Alex Margarit

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

APixelState a_pixel__state;
static AList* g_stateStack;

void a_pixel__init(void)
{
    a_pixel__state.blend = A_PIXEL_BLEND_PLAIN;
    g_stateStack = a_list_new();
}

void a_pixel__uninit(void)
{
    A_LIST_ITERATE(g_stateStack, APixelState*, state) {
        free(state);
    }

    a_list_free(g_stateStack);
}

void a_pixel_push(void)
{
    APixelState* state = a_mem_malloc(sizeof(APixelState));

    *state = a_pixel__state;
    a_list_push(g_stateStack, state);
}

void a_pixel_pop(void)
{
    APixelState* state = a_list_pop(g_stateStack);

    if(state == NULL) {
        a_out__fatal("Cannot pop APixelState: stack is empty");
    }

    a_pixel__state = *state;
    free(state);

    a_pixel_setBlend(a_pixel__state.blend);
    a_pixel_setRGBA(a_pixel__state.red,
                    a_pixel__state.green,
                    a_pixel__state.blue,
                    a_pixel__state.alpha);
}

void a_pixel_setBlend(APixelBlend Blend)
{
    a_pixel__state.blend = Blend;

    a_sprite__updateRoutines();
    a_draw__updateRoutines();
}

void a_pixel_setAlpha(unsigned int Alpha)
{
    a_pixel__state.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);
}

void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    a_pixel__state.red = Red;
    a_pixel__state.green = Green;
    a_pixel__state.blue = Blue;

    a_pixel__state.pixel = a_pixel_make(a_pixel__state.red,
                                        a_pixel__state.green,
                                        a_pixel__state.blue);
}

void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, unsigned int Alpha)
{
    a_pixel__state.red = Red;
    a_pixel__state.green = Green;
    a_pixel__state.blue = Blue;
    a_pixel__state.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);

    a_pixel__state.pixel = a_pixel_make(a_pixel__state.red,
                                        a_pixel__state.green,
                                        a_pixel__state.blue);
}

void a_pixel_setPixel(APixel Pixel)
{
    a_pixel__state.pixel = Pixel;

    a_pixel__state.red = a_pixel_red(a_pixel__state.pixel);
    a_pixel__state.green = a_pixel_green(a_pixel__state.pixel);
    a_pixel__state.blue = a_pixel_blue(a_pixel__state.pixel);
}
