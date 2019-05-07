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

#include "a2x_pack_screen.v.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

AScreen a__screen;
static AList* g_stack; // list of AScreen

#if A_CONFIG_TRAIT_DESKTOP
    static AButton* g_fullScreenButton;

    #define A__ZOOM_LEVELS 3
    static AButton* g_zoomButtons[A__ZOOM_LEVELS];

#endif

static void initScreen(AScreen* Screen, int Width, int Height, bool AllocBuffer)
{
    Screen->pixels = a_pixels__newEx(Width, Height, AllocBuffer);
    Screen->pixels->texture = a_platform_api__textureNewScreen(Screen->pixels);
    Screen->sprite = NULL;
    Screen->clipX = 0;
    Screen->clipY = 0;
    Screen->clipX2 = Width;
    Screen->clipY2 = Height;
    Screen->clipWidth = Width;
    Screen->clipHeight = Height;
}

static void freeScreen(AScreen* Screen)
{
    a_pixels__free(Screen->pixels);
}

void a_screen__init(void)
{
    int width = A_CONFIG_SCREEN_WIDTH;
    int height = A_CONFIG_SCREEN_HEIGHT;

    if(width < 0 || height < 0) {
        AVectorInt res = a_platform_api__screenResolutionGetNative();

        if(res.x > 0 && res.y > 0) {
            if(width < 0) {
                width = res.x / -width;
            }

            if(height < 0) {
                height = res.y / -height;
            }
        }
    }

    if(width <= 0 || height <= 0) {
        A__FATAL("Invalid screen resolution %dx%d", width, height);
    } else {
        a_out__info("Screen resolution %dx%d, zoom x%d",
                    width,
                    height,
                    A_CONFIG_SCREEN_ZOOM);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        // Allocate a pixel buffer, or use SDL's pixel buffer directly
        initScreen(&a__screen, width, height, A_CONFIG_SCREEN_ALLOCATE);
    #endif

    a_platform_api__screenInit(width, height);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        initScreen(&a__screen, width, height, true);
        a_platform_api__renderTargetSet(a__screen.pixels->texture);
    #endif

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

void a_screen__uninit(void)
{
    freeScreen(&a__screen);

    if(!a_list_isEmpty(g_stack)) {
        a_out__warning("Leaked %u screen targets", a_list_sizeGet(g_stack));
    }

    a_list_freeEx(g_stack, (AFree*)a_screen_free);

    #if A_CONFIG_TRAIT_DESKTOP
        a_button_free(g_fullScreenButton);

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            a_button_free(g_zoomButtons[z]);
        }
    #endif
}

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
        a_platform_api__renderTargetPixelsCapture(
            a__screen.pixels->buffer, a__screen.pixels->w);
    #endif

    return a__screen.pixels->buffer;
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

AScreen* a_screen_new(int Width, int Height)
{
    AScreen* s = a_mem_malloc(sizeof(AScreen));

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        initScreen(s, Width, Height, true);
    #else
        initScreen(s, Width, Height, false);
    #endif

    return s;
}

void a_screen_free(AScreen* Screen)
{
    if(Screen == NULL) {
        return;
    }

    freeScreen(Screen);
    free(Screen);
}

static inline bool sameSize(const AScreen* Screen1, const AScreen* Screen2)
{
    return Screen1->pixels->w == Screen2->pixels->w
        && Screen1->pixels->h == Screen2->pixels->h;
}

void a_screen_copy(AScreen* Dst, const AScreen* Src)
{
    if(!sameSize(Dst, Src)) {
        A__FATAL("a_screen_copy(%dx%d, %dx%d): Different sizes",
                 Dst->pixels->w,
                 Dst->pixels->h,
                 Src->pixels->w,
                 Src->pixels->h);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_pixels__copy(Dst->pixels, Src->pixels);
    #else
        a_color_push();
        a_color_blendSet(A_COLOR_BLEND_PLAIN);

        a_platform_api__renderTargetSet(Dst->pixels->texture);
        a_platform_api__renderTargetClipSet(
            0, 0, Dst->pixels->w, Dst->pixels->h);

        a_platform_api__textureBlit(Src->pixels->texture, 0, 0, false);

        a_platform_api__renderTargetSet(a__screen.pixels->texture);
        a_platform_api__renderTargetClipSet(a__screen.clipX,
                                            a__screen.clipY,
                                            a__screen.clipWidth,
                                            a__screen.clipHeight);

        a_color_pop();
    #endif
}

void a_screen_blit(const AScreen* Screen)
{
    if(!sameSize(&a__screen, Screen)) {
        A__FATAL("a_screen_blit(%dx%d): Current screen is %dx%d",
                 Screen->pixels->w,
                 Screen->pixels->h,
                 a__screen.pixels->w,
                 a__screen.pixels->h);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        bool noClipping = a_screen_boxInsideClip(
                            0, 0, a__screen.pixels->w, a__screen.pixels->h);
        APixel* dst = a__screen.pixels->buffer;
        APixel* src = Screen->pixels->buffer;
        ARgb rgb = {0, 0, 0};
        int alpha = a__color.alpha;

        #define LOOP(Blend, Setup, Params)                                    \
            if(noClipping) {                                                  \
                for(int i = Screen->pixels->w * Screen->pixels->h; i--; ) {   \
                    Setup;                                                    \
                    a_color__draw_##Blend Params;                             \
                    dst++;                                                    \
                    src++;                                                    \
                }                                                             \
            } else {                                                          \
                ptrdiff_t offset = a__screen.pixels->w - a__screen.clipWidth; \
                                                                              \
                dst +=                                                        \
                    a__screen.pixels->w * a__screen.clipY + a__screen.clipX;  \
                src +=                                                        \
                    a__screen.pixels->w * a__screen.clipY + a__screen.clipX;  \
                                                                              \
                for(int i = a__screen.clipHeight; i--; ) {                    \
                    for(int j = a__screen.clipWidth; j--; ) {                 \
                        Setup;                                                \
                        a_color__draw_##Blend Params;                         \
                        dst++;                                                \
                        src++;                                                \
                    }                                                         \
                                                                              \
                    dst += offset;                                            \
                    src += offset;                                            \
                }                                                             \
            }

        switch(a__color.blend) {
            case A_COLOR_BLEND_PLAIN: {
                if(noClipping) {
                    a_pixels__copy(a__screen.pixels, Screen->pixels);
                } else {
                    dst +=
                        a__screen.pixels->w * a__screen.clipY + a__screen.clipX;
                    src +=
                        a__screen.pixels->w * a__screen.clipY + a__screen.clipX;

                    for(int i = a__screen.clipHeight; i--; ) {
                        memcpy(dst,
                               src,
                               (unsigned)a__screen.clipWidth * sizeof(APixel));
                        dst += a__screen.pixels->w;
                        src += a__screen.pixels->w;
                    }
                }
            } break;

            case A_COLOR_BLEND_RGBA: {
                LOOP(rgba, {rgb = a_pixel_toRgb(*src);}, (dst, &rgb, alpha));
            } break;

            case A_COLOR_BLEND_RGB25: {
                LOOP(rgb25, {rgb = a_pixel_toRgb(*src);}, (dst, &rgb));
            } break;

            case A_COLOR_BLEND_RGB50: {
                LOOP(rgb50, {rgb = a_pixel_toRgb(*src);}, (dst, &rgb));
            } break;

            case A_COLOR_BLEND_RGB75: {
                LOOP(rgb75, {rgb = a_pixel_toRgb(*src);}, (dst, &rgb));
            } break;

            case A_COLOR_BLEND_INVERSE: {
                LOOP(inverse, {}, (dst));
            } break;

            case A_COLOR_BLEND_MOD: {
                LOOP(mod, {rgb = a_pixel_toRgb(*src);}, (dst, &rgb));
            } break;

            default: break;
        }
    #else
        a_platform_api__textureBlit(Screen->pixels->texture, 0, 0, false);
    #endif
}

void a_screen_clear(void)
{
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_pixels__clear(a__screen.pixels);
    #else
        a_color_push();

        a_color_blendSet(A_COLOR_BLEND_PLAIN);
        a_color_baseSetPixel(0);
        a_platform_api__screenClear();

        a_color_pop();
    #endif
}

void a_screen_targetPush(ASprite* Sprite)
{
    a_list_push(g_stack, a_mem_dup(&a__screen, sizeof(AScreen)));

    a__screen.pixels = Sprite->pixels;
    a__screen.sprite = Sprite;

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__renderTargetSet(a__screen.pixels->texture);
    #endif

    a_screen_clipReset();
}

void a_screen_targetPop(void)
{
    AScreen* screen = a_list_pop(g_stack);

    if(screen == NULL) {
        A__FATAL("a_screen_targetPop: Stack is empty");
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        if(a__screen.sprite) {
            a_pixels__commit(a__screen.sprite->pixels);
        }
    #endif

    a__screen = *screen;
    free(screen);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__renderTargetSet(a__screen.pixels->texture);
        a_platform_api__renderTargetClipSet(a__screen.clipX,
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
        a_platform_api__renderTargetClipSet(X, Y, Width, Height);
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
