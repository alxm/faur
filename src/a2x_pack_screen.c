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
static AScreen g_savedScreen;
static ASprite* g_spriteTarget = NULL;
static AList* g_overlays;
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

    #if A_CONFIG_RENDER_SDL2
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
    #endif

    g_savedScreen = a__screen;
    g_overlays = a_list_new();

    a_screen_setTargetScreen(&a__screen);
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

    if(a_settings_getBool("video.doubleBuffer")) {
        // Use g_savedScreen in case app forgot to call a_screen_resetTarget
        freeScreen(&g_savedScreen);
    }

    A_LIST_ITERATE(g_overlays, AScreenOverlayContainer*, c) {
        free(c);
    }

    a_list_free(g_overlays);
}

#if A_CONFIG_RENDER_SOFTWARE
void a_screen__setPixelBuffer(APixel* Pixels)
{
    a__screen.pixels = Pixels;
    g_savedScreen.pixels = Pixels;
}
#endif

void a_screen__show(void)
{
    #if A_CONFIG_RENDER_SOFTWARE
        if(a__screen.pixels != g_savedScreen.pixels) {
            a_out__fatal("Call a_screen_resetTarget before drawing frame");
        }
    #elif A_CONFIG_RENDER_SDL2
        if(a__screen.texture != g_savedScreen.texture) {
            a_out__fatal("Call a_screen_resetTarget before drawing frame");
        }
    #endif

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

        a_sdl_render__textureBlit(Src->texture,
                                  0,
                                  0,
                                  Src->width,
                                  Src->height,
                                  false);

        a_sdl_render__targetSet(a__screen.texture);
    #endif
}

void a_screen_blit(const AScreen* Screen)
{
    a_screen_copy(&a__screen, Screen);
}

#if A_CONFIG_RENDER_SOFTWARE
APixel a_screen_getPixel(int X, int Y)
{
    return *(a__screen.pixels + Y * a__screen.width + X);
}

void a_screen_setTargetBuffer(APixel* Pixels, int Width, int Height)
{
    a_screen_resetTarget();

    a__screen.pixels = Pixels;
    a__screen.width = Width;
    a__screen.height = Height;

    a_screen_resetClip();
}
#elif A_CONFIG_RENDER_SDL2
static void setRenderTarget(ASdlTexture* Texture, int Width, int Height)
{
    a_screen_resetTarget();

    a__screen.texture = Texture;
    a__screen.width = Width;
    a__screen.height = Height;

    a_screen_resetClip();
    a_sdl_render__targetSet(Texture);
}
#endif

void a_screen_setTargetScreen(AScreen* Screen)
{
    #if A_CONFIG_RENDER_SOFTWARE
        a_screen_setTargetBuffer(Screen->pixels, Screen->width, Screen->height);
    #elif A_CONFIG_RENDER_SDL2
        setRenderTarget(Screen->texture, Screen->width, Screen->height);
    #endif
}

void a_screen_setTargetSprite(ASprite* Sprite)
{
    #if A_CONFIG_RENDER_SOFTWARE
        a_screen_setTargetBuffer(Sprite->pixels, Sprite->w, Sprite->h);
        g_spriteTarget = Sprite;
    #elif A_CONFIG_RENDER_SDL2
        setRenderTarget(Sprite->texture, Sprite->w, Sprite->h);
    #endif
}

void a_screen_resetTarget(void)
{
    a__screen = g_savedScreen;

    #if A_CONFIG_RENDER_SDL2
        a_sdl_render__targetSet(a__screen.texture);
    #endif

    if(g_spriteTarget) {
        #if A_CONFIG_RENDER_SOFTWARE
            a_sprite__refreshTransparency(g_spriteTarget);
        #endif

        g_spriteTarget = NULL;
    }
}

void a_screen_setClip(int X, int Y, int Width, int Height)
{
    if(!a_screen_boxInsideScreen(X, Y, Width, Height)) {
        a_out__error("Invalid clipping area: (%d, %d) %d x %d",
                     X, Y, Width, Height);
        return;
    }

    a__screen.clipX = X;
    a__screen.clipY = Y;
    a__screen.clipX2 = X + Width;
    a__screen.clipY2 = Y + Height;
    a__screen.clipWidth = Width;
    a__screen.clipHeight = Height;
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
