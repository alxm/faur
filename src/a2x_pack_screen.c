/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_system_includes.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_platform_wiz.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"

typedef struct {
    AScreenOverlay callback;
} AScreenOverlayContainer;

AScreen a__screen;
static AList* g_stack; // list of AScreen

static AList* g_overlays; // list of AScreenOverlayContainer

static bool g_fullScreenState;

#if A_PLATFORM_SYSTEM_DESKTOP
    static AInputButton* g_fullScreenButton;

    static void inputCallback(void)
    {
        if(a_button_getPressedOnce(g_fullScreenButton)) {
            g_fullScreenState = !g_fullScreenState;
            a_platform__setFullScreen(g_fullScreenState);
        }
    }
#endif

static void initScreen(AScreen* Screen, int Width, int Height, bool AllocBuffer)
{
    Screen->pixelsSize = (unsigned)Width * (unsigned)Height * sizeof(APixel);

    if(AllocBuffer) {
        Screen->pixels = a_mem_zalloc(Screen->pixelsSize);
    } else {
        Screen->pixels = NULL;
    }

    Screen->sprite = NULL;
    Screen->texture = a_platform__newScreenTexture(Width, Height);
    Screen->width = Width;
    Screen->height = Height;
    Screen->clipX = 0;
    Screen->clipY = 0;
    Screen->clipX2 = Width;
    Screen->clipY2 = Height;
    Screen->clipWidth = Width;
    Screen->clipHeight = Height;
    Screen->ownsBuffer = AllocBuffer;
}

static void freeScreen(AScreen* Screen)
{
    if(Screen->ownsBuffer) {
        free(Screen->pixels);
    }

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__freeTexture(Screen->texture);
    #endif
}

void a_screen__init(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    int width = a_settings_getInt("video.width");
    int height = a_settings_getInt("video.height");

    if(width < 0 || height < 0) {
        int w = width;
        int h = height;

        a_platform__getNativeResolution(&w, &h);

        if(w > 0 && h > 0) {
            if(width < 0) {
                width = w / -width;
            }

            if(height < 0) {
                height = h / -height;
            }
        }
    }

    if(width <= 0 || height <= 0) {
        a_out__fatal("Invalid screen resolution %dx%d", width, height);
    }

    #if A_PLATFORM_RENDER_SOFTWARE
        if(a_settings_getBool("video.doubleBuffer")) {
            // Allocate pixel buffer
            initScreen(&a__screen, width, height, true);
        } else {
            // Will use SDL's pixel buffer directly
            initScreen(&a__screen, width, height, false);
        }
    #endif

    g_fullScreenState = a_settings_getBool("video.fullscreen");
    a_platform__setScreen(width, height, g_fullScreenState);
    a_platform__setFullScreen(g_fullScreenState);

    #if A_PLATFORM_SYSTEM_WIZ
        a_platform_wiz__setScreenPortraitMode();
    #endif

    #if A_PLATFORM_SYSTEM_DESKTOP
        g_fullScreenButton = a_button_new(
            a_settings_getString("video.fullscreen.button"));

        a_input__addCallback(inputCallback);
    #endif

    #if !A_PLATFORM_RENDER_SOFTWARE
        initScreen(&a__screen, width, height, true);
        a_platform__setRenderTarget(a__screen.texture);
    #endif

    g_stack = a_list_new();
    g_overlays = a_list_new();
}

void a_screen__uninit(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    freeScreen(&a__screen);

    if(!a_list_isEmpty(g_stack)) {
        a_out__warning("Leaked %u screen targets", a_list_getSize(g_stack));
    }

    a_list_freeEx(g_stack, (AFree*)a_screen_free);
    a_list_freeEx(g_overlays, free);
}

void a_screen__show(void)
{
    if(!a_list_isEmpty(g_stack)) {
        a_out__fatal("Call a_screen_targetPop for each a_screen_targetPush");
    }

    A_LIST_ITERATE(g_overlays, AScreenOverlayContainer*, c) {
        c->callback();
    }

    a_platform__showScreen();
}

void a_screen__addOverlay(AScreenOverlay Callback)
{
    AScreenOverlayContainer* c = a_mem_malloc(sizeof(AScreenOverlayContainer));
    c->callback = Callback;

    a_list_addLast(g_overlays, c);
}

bool a_screen__sameSize(const AScreen* Screen1, const AScreen* Screen2)
{
    return Screen1->width == Screen2->width
        && Screen1->height == Screen2->height;
}

APixel* a_screen_getPixels(void)
{
    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__getTargetPixels(a__screen.pixels, a__screen.width);
    #endif

    return a__screen.pixels;
}

int a_screen_getWidth(void)
{
    return a__screen.width;
}

int a_screen_getHeight(void)
{
    return a__screen.height;
}

AScreen* a_screen_new(int Width, int Height)
{
    AScreen* s = a_mem_malloc(sizeof(AScreen));

    initScreen(s, Width, Height, true);

    return s;
}

AScreen* a_screen_dup(void)
{
    AScreen* s = a_screen_new(a__screen.width, a__screen.height);

    a_screen_copy(s, &a__screen);

    return s;
}

void a_screen_free(AScreen* Screen)
{
    freeScreen(Screen);
    free(Screen);
}

void a_screen_copy(AScreen* Dst, const AScreen* Src)
{
    if(!a_screen__sameSize(Dst, Src)) {
        a_out__fatal("a_screen_copy: different screen sizes %d,%d and %d,%d",
                     Dst->width,
                     Dst->height,
                     Src->width,
                     Src->height);
    }

    #if A_PLATFORM_RENDER_SOFTWARE
        memcpy(Dst->pixels, Src->pixels, Src->pixelsSize);
    #else
        a_pixel_push();
        a_pixel_setBlend(A_PIXEL_BLEND_PLAIN);

        a_platform__setRenderTarget(Dst->texture);
        a_platform__setTargetClip(0, 0, Dst->width, Dst->height);

        a_platform__blitTexture(Src->texture, 0, 0, false);

        a_platform__setRenderTarget(a__screen.texture);
        a_platform__setTargetClip(a__screen.clipX,
                                  a__screen.clipY,
                                  a__screen.clipWidth,
                                  a__screen.clipHeight);

        a_pixel_pop();
    #endif
}

void a_screen_blit(const AScreen* Screen)
{
    if(!a_screen__sameSize(&a__screen, Screen)) {
        a_out__fatal("a_screen_blit: different screen sizes %d,%d and %d,%d",
                     a__screen.width,
                     a__screen.height,
                     Screen->width,
                     Screen->height);
    }

    #if A_PLATFORM_RENDER_SOFTWARE
        bool noClipping = a_screen_isBoxInsideClip(0,
                                                   0,
                                                   a__screen.width,
                                                   a__screen.height);
        APixel* dst = a__screen.pixels;
        APixel* src = Screen->pixels;
        APixel srcPixel;
        int alpha = a_pixel__state.alpha;

        #define LOOP(blend, params)                                         \
            if(noClipping) {                                                \
                for(int i = Screen->width * Screen->height; i--; ) {        \
                    srcPixel = *src;                                        \
                    a_pixel__##blend params;                                \
                    dst++;                                                  \
                    src++;                                                  \
                }                                                           \
            } else {                                                        \
                ptrdiff_t offset = a__screen.width - a__screen.clipWidth;   \
                                                                            \
                dst += a__screen.width * a__screen.clipY + a__screen.clipX; \
                src += a__screen.width * a__screen.clipY + a__screen.clipX; \
                                                                            \
                for(int i = a__screen.clipHeight; i--; ) {                  \
                    for(int j = a__screen.clipWidth; j--; ) {               \
                        srcPixel = *src;                                    \
                        a_pixel__##blend params;                            \
                        dst++;                                              \
                        src++;                                              \
                    }                                                       \
                                                                            \
                    dst += offset;                                          \
                    src += offset;                                          \
                }                                                           \
            }

        switch(a_pixel__state.blend) {
            case A_PIXEL_BLEND_PLAIN: {
                if(noClipping) {
                    memcpy(dst, src, Screen->pixelsSize);
                } else {
                    dst += a__screen.width * a__screen.clipY + a__screen.clipX;
                    src += a__screen.width * a__screen.clipY + a__screen.clipX;

                    for(int i = a__screen.clipHeight; i--; ) {
                        memcpy(dst,
                               src,
                               (unsigned)a__screen.clipWidth * sizeof(APixel));
                        dst += a__screen.width;
                        src += a__screen.width;
                    }
                }
            } break;

            case A_PIXEL_BLEND_RGBA: {
                LOOP(rgba, (dst,
                            a_pixel_red(srcPixel),
                            a_pixel_green(srcPixel),
                            a_pixel_blue(srcPixel),
                            alpha));
            } break;

            case A_PIXEL_BLEND_RGB25: {
                LOOP(rgb25, (dst,
                             a_pixel_red(srcPixel),
                             a_pixel_green(srcPixel),
                             a_pixel_blue(srcPixel)));
            } break;

            case A_PIXEL_BLEND_RGB50: {
                LOOP(rgb50, (dst,
                             a_pixel_red(srcPixel),
                             a_pixel_green(srcPixel),
                             a_pixel_blue(srcPixel)));
            } break;

            case A_PIXEL_BLEND_RGB75: {
                LOOP(rgb75, (dst,
                             a_pixel_red(srcPixel),
                             a_pixel_green(srcPixel),
                             a_pixel_blue(srcPixel)));
            } break;

            case A_PIXEL_BLEND_INVERSE: {
                LOOP(inverse, (dst));
            } break;

            case A_PIXEL_BLEND_MOD: {
                LOOP(mod, (dst,
                           a_pixel_red(srcPixel),
                           a_pixel_green(srcPixel),
                           a_pixel_blue(srcPixel)));
            } break;

            default: break;
        }
    #else
        a_platform__blitTexture(Screen->texture, 0, 0, false);
    #endif
}

void a_screen_clear(void)
{
    #if A_PLATFORM_RENDER_SOFTWARE
        memset(a__screen.pixels, 0, a__screen.pixelsSize);
    #else
        a_pixel_push();

        a_pixel_setBlend(A_PIXEL_BLEND_PLAIN);
        a_pixel_setPixel(0);
        a_platform__renderClear();

        a_pixel_pop();
    #endif
}

static void pushTarget(APixel* Pixels, size_t PixelsSize, int Width, int Height, APlatformTexture* Texture, ASprite* Sprite)
{
    a_list_push(g_stack, a_mem_dup(&a__screen, sizeof(AScreen)));

    a__screen.pixels = Pixels;
    a__screen.pixelsSize = PixelsSize;
    a__screen.sprite = Sprite;
    a__screen.texture = Texture;
    a__screen.width = Width;
    a__screen.height = Height;
    a__screen.ownsBuffer = false;

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__setRenderTarget(Texture);
    #endif

    a_screen_clipReset();
}

void a_screen_targetPushScreen(AScreen* Screen)
{
    pushTarget(Screen->pixels,
               Screen->pixelsSize,
               Screen->width,
               Screen->height,
               Screen->texture,
               NULL);
}

void a_screen_targetPushSprite(ASprite* Sprite)
{
    pushTarget(Sprite->pixels,
               Sprite->pixelsSize,
               Sprite->w,
               Sprite->h,
               Sprite->texture,
               Sprite);
}

void a_screen_targetPop(void)
{
    #if A_PLATFORM_RENDER_SOFTWARE
        if(a__screen.sprite) {
            a_platform__commitSpriteTexture(a__screen.sprite);
        }
    #endif

    AScreen* screen = a_list_pop(g_stack);

    if(screen == NULL) {
        a_out__fatal("a_screen_targetPop: stack is empty");
    }

    a__screen = *screen;
    free(screen);

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__setRenderTarget(a__screen.texture);
        a_platform__setTargetClip(a__screen.clipX,
                                  a__screen.clipY,
                                  a__screen.clipWidth,
                                  a__screen.clipHeight);
    #endif
}

void a_screen_clipSet(int X, int Y, int Width, int Height)
{
    if(!a_screen_isBoxInsideScreen(X, Y, Width, Height)) {
        a_out__error("Invalid clipping area %dx%d @ %d,%d in %dx%d screen",
                     Width,
                     Height,
                     X,
                     Y,
                     a__screen.width,
                     a__screen.height);
        return;
    }

    a__screen.clipX = X;
    a__screen.clipY = Y;
    a__screen.clipX2 = X + Width;
    a__screen.clipY2 = Y + Height;
    a__screen.clipWidth = Width;
    a__screen.clipHeight = Height;

    #if !A_PLATFORM_RENDER_SOFTWARE
        a_platform__setTargetClip(X, Y, Width, Height);
    #endif
}

void a_screen_clipReset(void)
{
    a_screen_clipSet(0, 0, a__screen.width, a__screen.height);
}

bool a_screen_isBoxOnScreen(int X, int Y, int W, int H)
{
    return a_collide_boxAndBox(X, Y, W, H,
                               0, 0, a__screen.width, a__screen.height);
}

bool a_screen_isBoxInsideScreen(int X, int Y, int W, int H)
{
    return X >= 0 && Y >= 0
        && X + W <= a__screen.width && Y + H <= a__screen.height;
}

bool a_screen_isBoxOnClip(int X, int Y, int W, int H)
{
    return a_collide_boxAndBox(X, Y, W, H,
                               a__screen.clipX, a__screen.clipY,
                               a__screen.clipWidth, a__screen.clipHeight);
}

bool a_screen_isBoxInsideClip(int X, int Y, int W, int H)
{
    return X >= a__screen.clipX && Y >= a__screen.clipY
        && X + W <= a__screen.clipX2 && Y + H <= a__screen.clipY2;
}
