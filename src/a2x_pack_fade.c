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

#include "a2x_pack_fade.v.h"

static bool g_fadePending;
static int g_framesDuration;
static APixel g_savedColor;
static APixel* g_screenBuffer;
static APixel* g_savedScreen;
static int g_savedWidth, g_savedHeight;

static A_STATE(a_fade__toColor);
static A_STATE(a_fade__fromColor);
static A_STATE(a_fade__screens);

static void updateCachedBuffer(bool UpdateSavedScreen)
{
    if(A_SCREEN_SIZE > g_savedWidth * g_savedHeight * sizeof(APixel)) {
        if(g_screenBuffer != NULL) {
            free(g_screenBuffer);
        }

        g_screenBuffer = a_mem_malloc(A_SCREEN_SIZE);

        if(UpdateSavedScreen) {
            if(g_savedScreen != NULL) {
                free(g_savedScreen);
            }

            g_savedScreen = a_mem_malloc(A_SCREEN_SIZE);
        }
    }

    if(UpdateSavedScreen) {
        if(g_savedScreen == NULL) {
            g_savedScreen = a_mem_malloc(A_SCREEN_SIZE);
        }

        // capture current screen
        a_screen_copy(g_savedScreen, a_screen__pixels);
    }

    g_savedWidth = a_screen__width;
    g_savedHeight = a_screen__height;
}

static void validateCachedBuffer(void)
{
    if(a_screen__width != g_savedWidth || a_screen__height != g_savedHeight) {
        a_out__fatal("Screen size changed before fading");
    }
}

void a_fade__init(void)
{
    g_fadePending = false;
    g_framesDuration = 0;

    g_savedColor = 0;
    g_screenBuffer = NULL;
    g_savedScreen = NULL;
    g_savedWidth = 0;
    g_savedHeight = 0;

    a_state_new("a__fadeToColor", a_fade__toColor);
    a_state_new("a__fadeFromColor", a_fade__fromColor);
    a_state_new("a__fadeScreens", a_fade__screens);
}

void a_fade__uninit(void)
{
    if(g_screenBuffer != NULL) {
        free(g_screenBuffer);
    }

    if(g_savedScreen != NULL) {
        free(g_savedScreen);
    }
}

void a_fade_toColor(int FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_toColor: fade pending, ignoring");
        return;
    }

    g_framesDuration = FramesDuration;
    g_savedColor = a_pixel__state.pixel;

    updateCachedBuffer(false);

    a_state_push("a__fadeToColor");
    g_fadePending = true;
}

void a_fade_fromColor(int FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_fromColor: fade pending, ignoring");
        return;
    }

    g_framesDuration = FramesDuration;
    g_savedColor = a_pixel__state.pixel;

    updateCachedBuffer(false);

    a_state_push("a__fadeFromColor");
    g_fadePending = true;
}

void a_fade_screens(int FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_screens: fade pending, ignoring");
        return;
    }

    g_framesDuration = FramesDuration;

    updateCachedBuffer(true);

    a_state_push("a__fadeScreens");
    g_fadePending = true;
}

static A_STATE(a_fade__toColor)
{
    A_STATE_BODY
    {
        validateCachedBuffer();

        AFix alpha = 0;
        AFix alpha_inc = a_fix_itofix(A_PIXEL_ALPHA_MAX) / g_framesDuration;

        a_pixel_push();
        a_pixel_setBlend(A_PIXEL_BLEND_RGBA);
        a_pixel_setPixel(g_savedColor);

        a_screen_copy(g_screenBuffer, a_screen__pixels);

        A_STATE_LOOP
        {
            a_screen_copy(a_screen__pixels, g_screenBuffer);

            a_pixel_setAlpha(a_fix_fixtoi(alpha));
            a_draw_fill();

            alpha += alpha_inc;

            if(alpha > a_fix_itofix(A_PIXEL_ALPHA_MAX)) {
                a_state_pop();
            }
        }

        a_pixel_pop();
        g_fadePending = false;
    }
}

static A_STATE(a_fade__fromColor)
{
    A_STATE_BODY
    {
        validateCachedBuffer();

        AFix alpha = a_fix_itofix(A_PIXEL_ALPHA_MAX);
        AFix alpha_inc = a_fix_itofix(A_PIXEL_ALPHA_MAX) / g_framesDuration;

        a_pixel_push();
        a_pixel_setBlend(A_PIXEL_BLEND_RGBA);
        a_pixel_setPixel(g_savedColor);

        a_screen_copy(g_screenBuffer, a_screen__pixels);

        A_STATE_LOOP
        {
            a_screen_copy(a_screen__pixels, g_screenBuffer);

            a_pixel_setAlpha(a_fix_fixtoi(alpha));
            a_draw_fill();

            alpha -= alpha_inc;

            if(alpha < 0) {
                a_state_pop();
            }
        }

        a_pixel_pop();
        g_fadePending = false;
    }
}

static A_STATE(a_fade__screens)
{
    A_STATE_BODY
    {
        validateCachedBuffer();

        AFix alpha = a_fix_itofix(A_PIXEL_ALPHA_MAX);
        AFix alpha_inc = a_fix_itofix(A_PIXEL_ALPHA_MAX) / g_framesDuration;
        ASprite* oldScreen = a_sprite_fromPixels(g_savedScreen,
                                                 a_screen__width,
                                                 a_screen__height);

        a_pixel_push();
        a_pixel_setBlend(A_PIXEL_BLEND_RGBA);

        a_screen_copy(g_screenBuffer, a_screen__pixels);

        A_STATE_LOOP
        {
            a_screen_copy(a_screen__pixels, g_screenBuffer);

            a_pixel_setAlpha(a_fix_fixtoi(alpha));
            a_sprite_blit(oldScreen, 0, 0);

            alpha -= alpha_inc;

            if(alpha < 0) {
                a_state_pop();
            }
        }

        a_pixel_pop();
        a_sprite_free(oldScreen);
        g_fadePending = false;
    }
}
