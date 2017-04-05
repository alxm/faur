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

#include "a2x_pack_screen.v.h"

typedef struct AScreenOverlayContainer {
    AScreenOverlay callback;
} AScreenOverlayContainer;

AScreen a__screen;
static AList* g_stack; // list of AScreen

static AList* g_overlays; // list of AScreenOverlayContainer

static bool g_fullScreenState;
static AInputButton* g_fullScreenButton;

static void initScreen(AScreen* Screen, int Width, int Height, bool AllocBuffer)
{
    if(AllocBuffer) {
        size_t size = (unsigned)Width * (unsigned)Height * sizeof(APixel);
        Screen->pixels = a_mem_zalloc(size);
    } else {
        Screen->pixels = NULL;
    }

    #if A_CONFIG_RENDER_SOFTWARE
        Screen->sprite = NULL;
    #elif A_CONFIG_RENDER_SDL2
        Screen->texture = a_sdl_render__textureMakeScreen(Width, Height);
    #endif

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

    #if A_CONFIG_RENDER_SDL2
        a_sdl_render__textureFree(Screen->texture);
    #endif
}

void a_screen__init(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    int width = a_settings_getInt("video.width");
    int height = a_settings_getInt("video.height");

    #if A_CONFIG_RENDER_SDL2
        if(width == 0 && height == 0) {
            a_sdl_video__getFullResolution(&width, &height);
        }
    #endif

    if(width == 0 || height == 0) {
        a_out__fatal("Invalid screen resolution %dx%d", width, height);
    }

    #if A_CONFIG_RENDER_SOFTWARE
        if(a_settings_getBool("video.doubleBuffer")) {
            initScreen(&a__screen, width, height, true);
        } else {
            // Will use SDL's pixel buffer directly
            initScreen(&a__screen, width, height, false);
        }
    #endif

    if(a_settings_getBool("video.window")) {
        a_sdl_screen__set(width, height);

        #if A_PLATFORM_WIZ
            if(a_settings_getBool("video.fixWizTearing")) {
                a_hw__setWizPortraitMode();
            }
        #endif
    }

    #if A_CONFIG_RENDER_SDL2
        initScreen(&a__screen, width, height, true);
        a_sdl_render__targetSet(a__screen.texture);
    #endif

    g_stack = a_list_new();
    g_overlays = a_list_new();
}

static void inputCallback(void)
{
    if(a_button_getOnce(g_fullScreenButton)) {
        g_fullScreenState = !g_fullScreenState;
        a_sdl_screen__setFullScreen(g_fullScreenState);
    }
}

void a_screen__init2(void)
{
    g_fullScreenState = false;
    g_fullScreenButton = a_button_new(
                             a_settings_getString("video.fullscreen.button"));
    a_input__addCallback(inputCallback);
}

void a_screen__uninit(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    freeScreen(&a__screen);

    if(!a_list_empty(g_stack)) {
        a_out__warning("Leaked %u screen targets", a_list_size(g_stack));

        A_LIST_ITERATE(g_stack, AScreen*, screen) {
            a_screen_free(screen);
        }
    }

    A_LIST_ITERATE(g_overlays, AScreenOverlayContainer*, c) {
        free(c);
    }

    a_list_free(g_stack);
    a_list_free(g_overlays);
}

void a_screen__show(void)
{
    if(!a_list_empty(g_stack)) {
        a_out__fatal("Call a_screen_targetPop for each a_screen_targetPush");
    }

    A_LIST_ITERATE(g_overlays, AScreenOverlayContainer*, c) {
        c->callback();
    }

    a_sdl_screen__show();
}

void a_screen__addOverlay(AScreenOverlay Callback)
{
    AScreenOverlayContainer* c = a_mem_malloc(sizeof(AScreenOverlayContainer));
    c->callback = Callback;

    a_list_addLast(g_overlays, c);
}

APixel* a_screen_pixels(void)
{
    #if A_CONFIG_RENDER_SDL2
        a_sdl_render__targetGetPixels(a__screen.pixels, a__screen.width);
    #endif

    return a__screen.pixels;
}

int a_screen_width(void)
{
    return a__screen.width;
}

int a_screen_height(void)
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
    #if A_CONFIG_RENDER_SOFTWARE
        memcpy(Dst->pixels,
               Src->pixels,
               (unsigned)Src->width * (unsigned)Src->height * sizeof(APixel));
    #elif A_CONFIG_RENDER_SDL2
        a_sdl_render__targetSet(Dst->texture);
        a_sdl_render__textureBlit(Src->texture, 0, 0, false);
        a_sdl_render__targetSet(a__screen.texture);
    #endif
}

void a_screen_blit(const AScreen* Screen)
{
    a_screen_copy(&a__screen, Screen);
}

static void pushTarget(APixel* Pixels, int Width, int Height, void* Data)
{
    a_list_push(g_stack, a_mem_dup(&a__screen, sizeof(AScreen)));

    a__screen.pixels = Pixels;
    a__screen.width = Width;
    a__screen.height = Height;
    a__screen.ownsBuffer = false;

    #if A_CONFIG_RENDER_SOFTWARE
        a__screen.sprite = Data;
    #elif A_CONFIG_RENDER_SDL2
        a__screen.texture = Data;
        a_sdl_render__targetSet(Data);
    #endif

    a_screen_resetClip();
}

void a_screen_targetPushScreen(AScreen* Screen)
{
    #if A_CONFIG_RENDER_SOFTWARE
        pushTarget(Screen->pixels, Screen->width, Screen->height, NULL);
    #elif A_CONFIG_RENDER_SDL2
        pushTarget(Screen->pixels, Screen->width, Screen->height, Screen->texture);
    #endif
}

void a_screen_targetPushSprite(ASprite* Sprite)
{
    #if A_CONFIG_RENDER_SOFTWARE
        pushTarget(Sprite->pixels, Sprite->w, Sprite->h, Sprite);
    #elif A_CONFIG_RENDER_SDL2
        pushTarget(Sprite->pixels, Sprite->w, Sprite->h, Sprite->texture);
    #endif
}

void a_screen_targetPop(void)
{
    #if A_CONFIG_RENDER_SOFTWARE
        if(a__screen.sprite) {
            a_sprite__refreshTransparency(a__screen.sprite);
        }
    #endif

    AScreen* screen = a_list_pop(g_stack);

    if(screen == NULL) {
        a_out__fatal("a_screen_targetPop: stack is empty");
    }

    a__screen = *screen;
    free(screen);

    #if A_CONFIG_RENDER_SDL2
        a_sdl_render__targetSet(a__screen.texture);
        a_sdl_render__targetSetClip(a__screen.clipX,
                                    a__screen.clipY,
                                    a__screen.clipWidth,
                                    a__screen.clipHeight);
    #endif
}

void a_screen_setClip(int X, int Y, int Width, int Height)
{
    if(!a_screen_boxInsideScreen(X, Y, Width, Height)) {
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

    #if A_CONFIG_RENDER_SDL2
        a_sdl_render__targetSetClip(X, Y, Width, Height);
    #endif
}

void a_screen_resetClip(void)
{
    a_screen_setClip(0, 0, a__screen.width, a__screen.height);
}

bool a_screen_fitsInside(const AScreen* Screen)
{
    return a__screen.width * a__screen.height <= Screen->width * Screen->height;
}

bool a_screen_sameSizeAs(const AScreen* Screen)
{
    return a__screen.width == Screen->width
        && a__screen.height == Screen->height;
}

bool a_screen_boxOnScreen(int X, int Y, int W, int H)
{
    return a_collide_boxAndBox(X, Y, W, H,
                               0, 0, a__screen.width, a__screen.height);
}

bool a_screen_boxInsideScreen(int X, int Y, int W, int H)
{
    return X >= 0 && Y >= 0
        && X + W <= a__screen.width && Y + H <= a__screen.height;
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
