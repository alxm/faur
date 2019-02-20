/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#include "a2x_pack_collide.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_settings.v.h"

AScreen a__screen;
static AList* g_stack; // list of AScreen

#if A_CONFIG_TRAIT_DESKTOP
    static AButton* g_fullScreenButton;

    #define A__ZOOM_LEVELS 3
    static AButton* g_zoomButtons[A__ZOOM_LEVELS];
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
    Screen->texture = a_platform__textureScreenNew(Width, Height);
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

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform__textureFree(Screen->texture);
    #endif
}

void a_screen__init(void)
{
    int width = A_CONFIG_SCREEN_WIDTH;
    int height = A_CONFIG_SCREEN_HEIGHT;

    if(width < 0 || height < 0) {
        int w = 0, h = 0;
        a_platform__screenResolutionGetNative(&w, &h);

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
        A__FATAL("Invalid screen resolution %dx%d", width, height);
    } else {
        a_out__message("Screen resolution %dx%d", width, height);
    }

    #if A_CONFIG_SYSTEM_WIZ
        if(a_settings_boolGet(A_SETTING_SYSTEM_WIZ_FIXTEARING)) {
            a_settings_boolSet(A_SETTING_VIDEO_DOUBLEBUFFER, true);
        }
    #endif

    #if A_CONFIG_LIB_SDL == 2
        a_settings_boolSet(A_SETTING_VIDEO_DOUBLEBUFFER, true);
    #endif

    if(a_settings_intGet(A_SETTING_VIDEO_ZOOM) > 1) {
        a_settings_boolSet(A_SETTING_VIDEO_DOUBLEBUFFER, true);
    }

    #if A_CONFIG_LIB_SDL == 2 || A_CONFIG_SYSTEM_EMSCRIPTEN
        if(a_settings_isDefault(A_SETTING_VIDEO_VSYNC)) {
            a_settings_boolSet(A_SETTING_VIDEO_VSYNC, true);
        }
    #endif

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        if(a_settings_boolGet(A_SETTING_VIDEO_DOUBLEBUFFER)) {
            // Allocate pixel buffer
            initScreen(&a__screen, width, height, true);
        } else {
            // Will use SDL's pixel buffer directly
            initScreen(&a__screen, width, height, false);
        }
    #endif

    a_platform__screenInit(width, height);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        initScreen(&a__screen, width, height, true);
        a_platform__renderTargetSet(a__screen.texture);
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
            a_settings_boolFlip(A_SETTING_VIDEO_FULLSCREEN);
        }

        for(int z = 0; z < A__ZOOM_LEVELS; z++) {
            if(a_button_pressGetOnce(g_zoomButtons[z])) {
                a_settings_intSet(A_SETTING_VIDEO_ZOOM, z + 1);
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

    a_platform__screenShow();
}

bool a_screen__sameSize(const AScreen* Screen1, const AScreen* Screen2)
{
    return Screen1->width == Screen2->width
        && Screen1->height == Screen2->height;
}

APixel* a_screen_pixelsGetBuffer(void)
{
    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        if(a__screen.pixels == NULL) {
            a__screen.pixels = a_mem_malloc(a__screen.pixelsSize);
            a__screen.ownsBuffer = true;
        }

        a_platform__renderTargetPixelsGet(a__screen.pixels, a__screen.width);
    #endif

    return a__screen.pixels;
}

AVectorInt a_screen_sizeGet(void)
{
    return (AVectorInt){a__screen.width, a__screen.height};
}

int a_screen_sizeGetWidth(void)
{
    return a__screen.width;
}

int a_screen_sizeGetHeight(void)
{
    return a__screen.height;
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

AScreen* a_screen_dup(void)
{
    AScreen* s = a_screen_new(a__screen.width, a__screen.height);

    a_screen_copy(s, &a__screen);

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

void a_screen_copy(AScreen* Dst, const AScreen* Src)
{
    if(!a_screen__sameSize(Dst, Src)) {
        A__FATAL("a_screen_copy(%dx%d, %dx%d): Different sizes",
                 Dst->width,
                 Dst->height,
                 Src->width,
                 Src->height);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        memcpy(Dst->pixels, Src->pixels, Src->pixelsSize);
    #else
        a_pixel_push();
        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);

        a_platform__renderTargetSet(Dst->texture);
        a_platform__renderTargetClipSet(0, 0, Dst->width, Dst->height);

        a_platform__textureBlit(Src->texture, 0, 0, false);

        a_platform__renderTargetSet(a__screen.texture);
        a_platform__renderTargetClipSet(a__screen.clipX,
                                        a__screen.clipY,
                                        a__screen.clipWidth,
                                        a__screen.clipHeight);

        a_pixel_pop();
    #endif
}

void a_screen_blit(const AScreen* Screen)
{
    if(!a_screen__sameSize(&a__screen, Screen)) {
        A__FATAL("a_screen_blit(%dx%d): Current screen is %dx%d",
                 Screen->width,
                 Screen->height,
                 a__screen.width,
                 a__screen.height);
    }

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        bool noClipping = a_screen_boxInsideClip(
                            0, 0, a__screen.width, a__screen.height);
        APixel* dst = a__screen.pixels;
        APixel* src = Screen->pixels;
        int r = 0, g = 0, b = 0;
        int alpha = a_pixel__state.alpha;

        #define LOOP(Blend, Setup, Params)                                  \
            if(noClipping) {                                                \
                for(int i = Screen->width * Screen->height; i--; ) {        \
                    Setup;                                                  \
                    a_pixel__##Blend Params;                                \
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
                        Setup;                                              \
                        a_pixel__##Blend Params;                            \
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
                LOOP(rgba, {a_pixel_toRgb(*src, &r, &g, &b);}, (dst, r, g, b, alpha));
            } break;

            case A_PIXEL_BLEND_RGB25: {
                LOOP(rgb25, {a_pixel_toRgb(*src, &r, &g, &b);}, (dst, r, g, b));
            } break;

            case A_PIXEL_BLEND_RGB50: {
                LOOP(rgb50, {a_pixel_toRgb(*src, &r, &g, &b);}, (dst, r, g, b));
            } break;

            case A_PIXEL_BLEND_RGB75: {
                LOOP(rgb75, {a_pixel_toRgb(*src, &r, &g, &b);}, (dst, r, g, b));
            } break;

            case A_PIXEL_BLEND_INVERSE: {
                LOOP(inverse, {}, (dst));
            } break;

            case A_PIXEL_BLEND_MOD: {
                LOOP(mod, {a_pixel_toRgb(*src, &r, &g, &b);}, (dst, r, g, b));
            } break;

            default: break;
        }
    #else
        a_platform__textureBlit(Screen->texture, 0, 0, false);
    #endif
}

void a_screen_clear(void)
{
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        memset(a__screen.pixels, 0, a__screen.pixelsSize);
    #else
        a_pixel_push();

        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);
        a_pixel_colorSetPixel(0);
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

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform__renderTargetSet(Texture);
    #endif

    a_screen_clipReset();
}

void a_screen_targetPushScreen(const AScreen* Screen)
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
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        if(a__screen.sprite) {
            a__screen.sprite->texture = a_platform__textureSpriteNew(
                                            a__screen.sprite);
        }
    #endif

    AScreen* screen = a_list_pop(g_stack);

    if(screen == NULL) {
        A__FATAL("a_screen_targetPop: Stack is empty");
    }

    a__screen = *screen;
    free(screen);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform__renderTargetSet(a__screen.texture);
        a_platform__renderTargetClipSet(a__screen.clipX,
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

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform__renderTargetClipSet(X, Y, Width, Height);
    #endif
}

void a_screen_clipReset(void)
{
    a_screen_clipSet(0, 0, a__screen.width, a__screen.height);
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
