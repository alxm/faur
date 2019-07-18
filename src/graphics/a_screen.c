/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_screen.v.h"
#include <a2x.v.h>

AScreen a__screen;
static AList* g_stack; // list of AScreen

#if A_CONFIG_TRAIT_DESKTOP
    static AButton* g_fullScreenButton;

    #define A__ZOOM_LEVELS 3
    static AButton* g_zoomButtons[A__ZOOM_LEVELS];
#endif

static void a_screen__init(void)
{
    AVectorInt size = a_platform_api__screenSizeGet();

    a__screen.pixels = a_platform_api__screenPixelsGet();

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a__screen.texture = a_platform_api__screenTextureGet();
    #endif

    a__screen.clipX = 0;
    a__screen.clipY = 0;
    a__screen.clipX2 = size.x;
    a__screen.clipY2 = size.y;
    a__screen.clipWidth = size.x;
    a__screen.clipHeight = size.y;

    #if A_CONFIG_TRAIT_DESKTOP
        g_fullScreenButton = a_button_new();
        a_button_bind(g_fullScreenButton, A_KEY_F4);

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            g_zoomButtons[z] = a_button_new();
            a_button_bind(g_zoomButtons[z], A_KEY_F1 + z);
        }
    #endif

    g_stack = a_list_new();
}

static void a_screen__uninit(void)
{
    a_list_freeEx(g_stack, free);

    #if A_CONFIG_TRAIT_DESKTOP
        a_button_free(g_fullScreenButton);

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            a_button_free(g_zoomButtons[z]);
        }
    #endif
}

const APack a_pack__screen = {
    "Screen",
    {
        [0] = a_screen__init,
    },
    {
        [0] = a_screen__uninit,
    },
};

void a_screen__tick(void)
{
    #if A_CONFIG_TRAIT_DESKTOP
        if(a_button_pressGetOnce(g_fullScreenButton)) {
            a_platform_api__screenFullscreenFlip();

            a_out__info("Screen is now %s",
                        a_platform_api__screenFullscreenGet()
                            ? "fullscreen" : "windowed");
        }

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            if(a_button_pressGetOnce(g_zoomButtons[z])) {
                int zoom = z + 1;

                if(a_platform_api__screenZoomGet() != zoom) {
                    a_platform_api__screenZoomSet(zoom);

                    a_out__info(
                        "Screen zoom %d", a_platform_api__screenZoomGet());
                }

                break;
            }
        }
    #endif
}

void a_screen__draw(void)
{
    if(!a_list_isEmpty(g_stack)) {
        A__FATAL("Screen target stack is not empty");
    }

    a_platform_api__screenShow();
}

APixel* a_screen_pixelsGetBuffer(void)
{
    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__screenTextureRead(a__screen.pixels, a__screen.frame);
    #endif

    return a_screen__bufferGetFrom(0, 0);
}

AVectorInt a_screen_sizeGet(void)
{
    return (AVectorInt){a__screen.pixels->w, a__screen.pixels->h};
}

int a_screen_sizeGetWidth(void)
{
    return a__screen.pixels->w;
}

int a_screen_sizeGetHeight(void)
{
    return a__screen.pixels->h;
}

void a_screen_clear(void)
{
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_pixels__clear(a__screen.pixels, a__screen.frame);
    #else
        a_color_push();

        a_color_blendSet(A_COLOR_BLEND_PLAIN);
        a_color_baseSetPixel(0);
        a_platform_api__screenClear();

        a_color_pop();
    #endif
}

void a_screen_push(ASprite* Sprite, unsigned Frame)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(a_sprite__pixelsGet(Sprite)->flags,
                           A_PIXELS__CONST)) {

            A__FATAL("a_screen_push: Const sprite");
        }
    #endif

    a_list_push(g_stack, a_mem_dup(&a__screen, sizeof(AScreen)));

    a__screen.pixels = a_sprite__pixelsGet(Sprite);
    a__screen.sprite = Sprite;
    a__screen.frame = Frame;

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a__screen.texture = a_sprite__textureGet(Sprite, Frame);
        a_platform_api__screenTextureSet(a__screen.texture);

        A_FLAG_SET(a_sprite__pixelsGet(Sprite)->flags, A_PIXELS__DIRTY);
    #endif

    a_screen_clipReset();
}

void a_screen_pop(void)
{
    AScreen* screen = a_list_pop(g_stack);

    if(screen == NULL) {
        A__FATAL("a_screen_pop: Stack is empty");
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_sprite__textureCommit(a__screen.sprite, a__screen.frame);
    #endif

    a__screen = *screen;
    free(screen);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__screenTextureSet(a__screen.texture);
        a_platform_api__screenClipSet(a__screen.clipX,
                                      a__screen.clipY,
                                      a__screen.clipWidth,
                                      a__screen.clipHeight);
    #endif
}

void a_screen_clipSet(int X, int Y, int Width, int Height)
{
    if(!a_screen_boxInsideScreen(X, Y, Width, Height)) {
        a_out__error(
            "a_screen_clipSet(%d, %d, %d, %d): Invalid area on %dx%d screen",
            X,
            Y,
            Width,
            Height,
            a__screen.pixels->w,
            a__screen.pixels->h);

        return;
    }

    a__screen.clipX = X;
    a__screen.clipY = Y;
    a__screen.clipX2 = X + Width;
    a__screen.clipY2 = Y + Height;
    a__screen.clipWidth = Width;
    a__screen.clipHeight = Height;

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__screenClipSet(X, Y, Width, Height);
    #endif
}

void a_screen_clipReset(void)
{
    a_screen_clipSet(0, 0, a__screen.pixels->w, a__screen.pixels->h);
}

bool a_screen_boxOnScreen(int X, int Y, int W, int H)
{
    return a_collide_boxAndBox(X, Y, W, H,
                               0, 0, a__screen.pixels->w, a__screen.pixels->h);
}

bool a_screen_boxInsideScreen(int X, int Y, int W, int H)
{
    return X >= 0 && Y >= 0
        && X + W <= a__screen.pixels->w && Y + H <= a__screen.pixels->h;
}

bool a_screen_boxOnClip(int X, int Y, int W, int H)
{
    return a_collide_boxAndBox(X, Y, W, H,
                               a__screen.clipX, a__screen.clipY,
                               a__screen.clipWidth, a__screen.clipHeight);
}

bool a_screen_boxInsideClip(int X, int Y, int W, int H)
{
    return X >= a__screen.clipX && Y >= a__screen.clipY
        && X + W <= a__screen.clipX2 && Y + H <= a__screen.clipY2;
}

void a_screen__toSprite(ASprite* Sprite, unsigned Frame)
{
    AVectorInt spriteSize = a_sprite_sizeGet(Sprite);

    if(a__screen.pixels->w != spriteSize.x
        || a__screen.pixels->h != spriteSize.y) {

        A__FATAL("a_screen__toSprite: Sprite is %dx%d, screen is %dx%d",
                 spriteSize.x,
                 spriteSize.y,
                 a__screen.pixels->w,
                 a__screen.pixels->h);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_pixels__copyFrame(
            a_sprite__pixelsGet(Sprite), Frame, a__screen.pixels, 0);
    #else
        a_color_push();
        a_color_blendSet(A_COLOR_BLEND_PLAIN);
        a_color_fillBlitSet(false);

        a_platform_api__screenTextureSet(a_sprite__textureGet(Sprite, Frame));
        a_platform_api__screenClipSet(0, 0, spriteSize.x, spriteSize.y);

        a_platform_api__screenDraw();

        a_platform_api__screenTextureSet(a__screen.texture);
        a_platform_api__screenClipSet(a__screen.clipX,
                                      a__screen.clipY,
                                      a__screen.clipWidth,
                                      a__screen.clipHeight);

        a_color_pop();
    #endif
}
