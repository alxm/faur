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

#include "f_screen.v.h"
#include <faur.v.h>

AScreen f__screen;
static AList* g_stack; // list of AScreen

#if A_CONFIG_TRAIT_DESKTOP
    static AButton* g_fullScreenButton;

    #define A__ZOOM_LEVELS 3
    static AButton* g_zoomButtons[A__ZOOM_LEVELS];
#endif

static void f_screen__init(void)
{
    AVectorInt size = f_platform_api__screenSizeGet();

    f__screen.pixels = f_platform_api__screenPixelsGet();

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        f__screen.texture = f_platform_api__screenTextureGet();
    #endif

    f__screen.clipX = 0;
    f__screen.clipY = 0;
    f__screen.clipX2 = size.x;
    f__screen.clipY2 = size.y;
    f__screen.clipWidth = size.x;
    f__screen.clipHeight = size.y;

    #if A_CONFIG_TRAIT_DESKTOP
        g_fullScreenButton = f_button_new();
        f_button_bindKey(g_fullScreenButton, A_KEY_F4);

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            g_zoomButtons[z] = f_button_new();
            f_button_bindKey(g_zoomButtons[z], A_KEY_F1 + z);
        }
    #endif

    g_stack = f_list_new();
}

static void f_screen__uninit(void)
{
    f_list_freeEx(g_stack, f_mem_free);

    #if A_CONFIG_TRAIT_DESKTOP
        f_button_free(g_fullScreenButton);

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            f_button_free(g_zoomButtons[z]);
        }
    #endif
}

const APack f_pack__screen = {
    "Screen",
    {
        [0] = f_screen__init,
    },
    {
        [0] = f_screen__uninit,
    },
};

void f_screen__tick(void)
{
    #if A_CONFIG_TRAIT_DESKTOP
        if(f_button_pressGetOnce(g_fullScreenButton)) {
            f_platform_api__screenFullscreenFlip();

            f_out__info("Screen is now %s",
                        f_platform_api__screenFullscreenGet()
                            ? "fullscreen" : "windowed");
        }

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            if(f_button_pressGetOnce(g_zoomButtons[z])) {
                int zoom = z + 1;

                if(f_platform_api__screenZoomGet() != zoom) {
                    f_platform_api__screenZoomSet(zoom);

                    f_out__info(
                        "Screen zoom %d", f_platform_api__screenZoomGet());
                }

                break;
            }
        }
    #endif
}

void f_screen__draw(void)
{
    #if A_CONFIG_BUILD_DEBUG
        if(!f_list_isEmpty(g_stack)) {
            A__FATAL("Screen target stack is not empty");
        }
    #endif

    f_platform_api__screenShow();
}

APixel* f_screen_pixelsGetBuffer(void)
{
    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        f_platform_api__screenTextureRead(f__screen.pixels, f__screen.frame);
    #endif

    return f_screen__bufferGetFrom(0, 0);
}

AVectorInt f_screen_sizeGet(void)
{
    return (AVectorInt){f__screen.pixels->w, f__screen.pixels->h};
}

int f_screen_sizeGetWidth(void)
{
    return f__screen.pixels->w;
}

int f_screen_sizeGetHeight(void)
{
    return f__screen.pixels->h;
}

void f_screen_clear(void)
{
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        f_pixels__clear(f__screen.pixels, f__screen.frame);
    #else
        f_color_push();

        f_color_blendSet(A_COLOR_BLEND_PLAIN);
        f_color_baseSetPixel(0);
        f_platform_api__screenClear();

        f_color_pop();
    #endif
}

void f_screen_push(ASprite* Sprite, unsigned Frame)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(
            f_sprite__pixelsGet(Sprite)->flags, A_PIXELS__CONST)) {

            A__FATAL("f_screen_push: Const sprite");
        }
    #endif

    f_list_push(g_stack, f_mem_dup(&f__screen, sizeof(AScreen)));

    f__screen.pixels = f_sprite__pixelsGet(Sprite);
    f__screen.sprite = Sprite;
    f__screen.frame = Frame;

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        f__screen.texture = f_sprite__textureGet(Sprite, Frame);
        f_platform_api__screenTextureSet(f__screen.texture);

        A_FLAGS_SET(f_sprite__pixelsGet(Sprite)->flags, A_PIXELS__DIRTY);
    #endif

    f_screen_clipReset();
}

void f_screen_pop(void)
{
    AScreen* screen = f_list_pop(g_stack);

    #if A_CONFIG_BUILD_DEBUG
        if(screen == NULL) {
            A__FATAL("f_screen_pop: Stack is empty");
        }
    #endif

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        f_sprite__textureCommit(f__screen.sprite, f__screen.frame);
    #endif

    f__screen = *screen;
    f_mem_free(screen);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        f_platform_api__screenTextureSet(f__screen.texture);
        f_platform_api__screenClipSet(f__screen.clipX,
                                      f__screen.clipY,
                                      f__screen.clipWidth,
                                      f__screen.clipHeight);
    #endif
}

void f_screen_clipSet(int X, int Y, int Width, int Height)
{
    if(!f_screen_boxInsideScreen(X, Y, Width, Height)) {
        f_out__error(
            "f_screen_clipSet(%d, %d, %d, %d): Invalid area on %dx%d screen",
            X,
            Y,
            Width,
            Height,
            f__screen.pixels->w,
            f__screen.pixels->h);

        return;
    }

    f__screen.clipX = X;
    f__screen.clipY = Y;
    f__screen.clipX2 = X + Width;
    f__screen.clipY2 = Y + Height;
    f__screen.clipWidth = Width;
    f__screen.clipHeight = Height;

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        f_platform_api__screenClipSet(X, Y, Width, Height);
    #endif
}

void f_screen_clipReset(void)
{
    f_screen_clipSet(0, 0, f__screen.pixels->w, f__screen.pixels->h);
}

bool f_screen_boxOnScreen(int X, int Y, int W, int H)
{
    return f_collide_boxAndBox(X, Y, W, H,
                               0, 0, f__screen.pixels->w, f__screen.pixels->h);
}

bool f_screen_boxInsideScreen(int X, int Y, int W, int H)
{
    return X >= 0 && Y >= 0
        && X + W <= f__screen.pixels->w && Y + H <= f__screen.pixels->h;
}

bool f_screen_boxOnClip(int X, int Y, int W, int H)
{
    return f_collide_boxAndBox(X, Y, W, H,
                               f__screen.clipX, f__screen.clipY,
                               f__screen.clipWidth, f__screen.clipHeight);
}

bool f_screen_boxInsideClip(int X, int Y, int W, int H)
{
    return X >= f__screen.clipX && Y >= f__screen.clipY
        && X + W <= f__screen.clipX2 && Y + H <= f__screen.clipY2;
}

void f_screen__toSprite(ASprite* Sprite, unsigned Frame)
{
    AVectorInt spriteSize = f_sprite_sizeGet(Sprite);

    if(f__screen.pixels->w != spriteSize.x
        || f__screen.pixels->h != spriteSize.y) {

        A__FATAL("f_screen__toSprite: Sprite is %dx%d, screen is %dx%d",
                 spriteSize.x,
                 spriteSize.y,
                 f__screen.pixels->w,
                 f__screen.pixels->h);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        f_pixels__copyFrame(
            f_sprite__pixelsGet(Sprite), Frame, f__screen.pixels, 0);
    #else
        f_color_push();
        f_color_blendSet(A_COLOR_BLEND_PLAIN);
        f_color_fillBlitSet(false);

        f_platform_api__screenTextureSet(f_sprite__textureGet(Sprite, Frame));
        f_platform_api__screenClipSet(0, 0, spriteSize.x, spriteSize.y);

        f_platform_api__screenDraw();

        f_platform_api__screenTextureSet(f__screen.texture);
        f_platform_api__screenClipSet(f__screen.clipX,
                                      f__screen.clipY,
                                      f__screen.clipWidth,
                                      f__screen.clipHeight);

        f_color_pop();
    #endif
}
