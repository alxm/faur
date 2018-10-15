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
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_state.v.h"

static bool g_fadePending;
static unsigned g_frames;
static APixel g_savedColor;
static AScreen* g_capturedScreen;
static AScreen* g_oldCapturedScreen;

enum {
    A__FADE_TOCOLOR,
    A__FADE_FROMCOLOR,
    A__FADE_SCREENS,
    A__FADE_NUM
};

static AState* g_states[A__FADE_NUM];

static A_STATE(a_fade__toColor);
static A_STATE(a_fade__fromColor);
static A_STATE(a_fade__screens);

static void allocateScreenBuffers(bool CaptureCurrentScreen)
{
    if(g_capturedScreen == NULL
        || !a_screen__sameSize(&a__screen, g_capturedScreen)) {

        a_screen_free(g_capturedScreen);
        g_capturedScreen = a_screen_new(a__screen.width, a__screen.height);

        a_screen_free(g_oldCapturedScreen);
        g_oldCapturedScreen = NULL;
    }

    if(CaptureCurrentScreen) {
        if(g_oldCapturedScreen == NULL) {
            g_oldCapturedScreen = a_screen_new(
                                    a__screen.width, a__screen.height);
        }

        // Capture the screen before the caller will draw something new
        a_screen_copy(g_oldCapturedScreen, &a__screen);
    }
}

static void updateCapturedScreenBuffer(void)
{
    if(!a_screen__sameSize(&a__screen, g_capturedScreen)) {
        a_out__fatal("Screen size changed before fading");
    }

    a_screen_copy(g_capturedScreen, &a__screen);
}

void a_fade__init(void)
{
    g_fadePending = false;
    g_frames = 0;

    g_savedColor = 0;
    g_capturedScreen = NULL;
    g_oldCapturedScreen = NULL;

    g_states[A__FADE_TOCOLOR] =
        a_state_new("Fade to Color", a_fade__toColor);

    g_states[A__FADE_FROMCOLOR] =
        a_state_new("Fade from Color", a_fade__fromColor);

    g_states[A__FADE_SCREENS] =
        a_state_new("Fade Between Screens", a_fade__screens);
}

void a_fade__uninit(void)
{
    a_screen_free(g_capturedScreen);
    a_screen_free(g_oldCapturedScreen);
}

void a_fade_toColor(unsigned FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_toColor: Fade pending");
        return;
    }

    g_frames = FramesDuration;
    g_savedColor = a_pixel__state.pixel;
    allocateScreenBuffers(false);

    a_state_push(g_states[A__FADE_TOCOLOR]);
    g_fadePending = true;
}

void a_fade_fromColor(unsigned FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_fromColor: Fade pending");
        return;
    }

    g_frames = FramesDuration;
    g_savedColor = a_pixel__state.pixel;
    allocateScreenBuffers(false);

    a_state_push(g_states[A__FADE_FROMCOLOR]);
    g_fadePending = true;
}

void a_fade_screens(unsigned FramesDuration)
{
    if(g_fadePending) {
        a_out__warning("a_fade_screens: Fade pending");
        return;
    }

    g_frames = FramesDuration;
    allocateScreenBuffers(true);

    a_state_push(g_states[A__FADE_SCREENS]);
    g_fadePending = true;
}

static A_STATE(a_fade__toColor)
{
    static AFix alpha, alpha_inc;

    A_STATE_INIT
    {
        updateCapturedScreenBuffer();

        alpha = 0;
        alpha_inc = a_fix_fromInt(A_PIXEL_ALPHA_MAX) / (int)g_frames;

        a_pixel_push();
        a_pixel_colorSetPixel(g_savedColor);
    }

    A_STATE_TICK
    {
        alpha += alpha_inc;

        if(alpha > a_fix_fromInt(A_PIXEL_ALPHA_MAX)) {
            a_state_pop();
        }
    }

    A_STATE_DRAW
    {
        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);
        a_screen_blit(g_capturedScreen);

        a_pixel_blendSet(A_PIXEL_BLEND_RGBA);
        a_pixel_alphaSet(a_fix_toInt(alpha));
        a_draw_fill();
    }

    A_STATE_FREE
    {
        a_pixel_pop();
        g_fadePending = false;
    }
}

static A_STATE(a_fade__fromColor)
{
    static AFix alpha, alpha_inc;

    A_STATE_INIT
    {
        updateCapturedScreenBuffer();

        a_pixel_push();
        a_pixel_colorSetPixel(g_savedColor);

        alpha = a_fix_fromInt(A_PIXEL_ALPHA_MAX);
        alpha_inc = a_fix_fromInt(A_PIXEL_ALPHA_MAX) / (int)g_frames;
    }

    A_STATE_TICK
    {
        alpha -= alpha_inc;

        if(alpha < 0) {
            a_state_pop();
        }
    }

    A_STATE_DRAW
    {
        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);
        a_screen_blit(g_capturedScreen);

        a_pixel_blendSet(A_PIXEL_BLEND_RGBA);
        a_pixel_alphaSet(a_fix_toInt(alpha));
        a_draw_fill();
    }

    A_STATE_FREE
    {
        a_pixel_pop();
        g_fadePending = false;
    }
}

static A_STATE(a_fade__screens)
{
    static AFix alpha, alpha_inc;

    A_STATE_INIT
    {
        updateCapturedScreenBuffer();

        alpha = a_fix_fromInt(A_PIXEL_ALPHA_MAX);
        alpha_inc = a_fix_fromInt(A_PIXEL_ALPHA_MAX) / (int)g_frames;

        a_pixel_push();
    }

    A_STATE_TICK
    {
        alpha -= alpha_inc;

        if(alpha < 0) {
            a_state_pop();
        }
    }

    A_STATE_DRAW
    {
        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);
        a_screen_blit(g_capturedScreen);

        a_pixel_blendSet(A_PIXEL_BLEND_RGBA);
        a_pixel_alphaSet(a_fix_toInt(alpha));
        a_screen_blit(g_oldCapturedScreen);
    }

    A_STATE_FREE
    {
        a_pixel_pop();
        g_fadePending = false;
    }
}
