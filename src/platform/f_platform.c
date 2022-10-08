/*
    Copyright 2017 Alex Margarit <alex@alxm.org>
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

#include "f_platform.v.h"
#include <faur.v.h>

#if F_CONFIG_SYSTEM_ARDUINO
    //
    // arduino-builder workaround: for apps, only the source files from the
    // root source directory are built, so include nested generated files here.
    //
    #include <faur_v/include.c>
#endif

#include "files/f_gamebuino_file.v.h"
#include "files/f_odroid_go_file.v.h"
#include "files/f_standard_dir.v.h"
#include "files/f_standard_file.v.h"

#include "graphics/f_sdl_blit.v.h"
#include "graphics/f_sdl_draw.v.h"
#include "graphics/f_software_blit.v.h"
#include "graphics/f_software_draw.v.h"

#include "input/f_gamebuino_input.v.h"
#include "input/f_odroid_go_input.v.h"
#include "input/f_sdl_input.v.h"

#include "memory/f_odroidgo_malloc.v.h"
#include "memory/f_standard_malloc.v.h"

#include "sound/f_gamebuino_sound.v.h"
#include "sound/f_sdl_sound.v.h"

#include "system/f_emscripten.v.h"
#include "system/f_gamebuino.v.h"
#include "system/f_gp2x.v.h"
#include "system/f_linux.v.h"
#include "system/f_odroid_go.v.h"
#include "system/f_pandora.v.h"
#include "system/f_sdl.v.h"
#include "system/f_wiz.v.h"

#include "video/f_gamebuino_video.v.h"
#include "video/f_odroid_go_video.v.h"
#include "video/f_sdl_video.v.h"

static void f_platform__init(void)
{
    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        f_platform_emscripten__init();
    #endif

    #if F_CONFIG_SYSTEM_LINUX
        f_platform_linux__init();
    #endif

    #if F_CONFIG_SYSTEM_PANDORA
        f_platform_pandora__init();
    #endif

    #if F_CONFIG_LIB_SDL
        f_platform_sdl__init();
    #endif

    #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        #if F_CONFIG_SYSTEM_GP2X
            f_platform_gp2x__init();
        #else
            f_platform_wiz__init();
        #endif

        #if F_CONFIG_SYSTEM_GP2X_MENU
            atexit(f_platform_gp2x__execMenu);
        #endif
    #endif

    #if F_CONFIG_SYSTEM_ODROID_GO
        f_platform_odroid_go__init();
    #endif

    f_platform_api__screenInit();

    #if F_CONFIG_SCREEN_RENDER_SOFTWARE
        f_out__info("Using S/W graphics");
        f_platform_software_blit__init();
    #elif F_CONFIG_SCREEN_RENDER_SDL2
        f_out__info("Using SDL2 graphics");
    #endif
}

static void f_platform__uninit(void)
{
    #if F_CONFIG_SCREEN_RENDER_SOFTWARE
        f_platform_software_blit__uninit();
    #endif

    f_platform_api__screenUninit();

    #if F_CONFIG_SYSTEM_GP2X
        f_platform_gp2x__uninit();
    #elif F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        f_platform_wiz__uninit();
    #elif F_CONFIG_SYSTEM_PANDORA
        f_platform_pandora__uninit();
    #endif

    #if F_CONFIG_LIB_SDL
        f_platform_sdl__uninit();
    #endif
}

const FPack f_pack__platform = {
    "Platform",
    f_platform__init,
    f_platform__uninit,
};

static const FPlatformApi f__platform_api = {
    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        .customExit = f_platform_api_emscripten__customExit,
    #endif

    #if F_CONFIG_LIB_SDL && F_CONFIG_LIB_SDL_TIME
        .timeMsGet = f_platform_api_sdl__timeMsGet,
        .timeMsWait = f_platform_api_sdl__timeMsWait,
    #elif F_CONFIG_SYSTEM_GAMEBUINO
        .timeMsGet = f_platform_api_gamebuino__timeMsGet,
        .timeMsWait = f_platform_api_gamebuino__timeMsWait,
    #elif F_CONFIG_SYSTEM_ODROID_GO
        .timeMsGet = f_platform_api_odroidgo__timeMsGet,
        .timeMsWait = f_platform_api_odroidgo__timeMsWait,
    #elif F_CONFIG_SYSTEM_WIZ
        .timeMsGet = f_platform_api_wiz__timeMsGet,
        .timeMsWait = f_platform_api_wiz___timeMsWait,
    #endif

    #if F_CONFIG_LIB_SDL
        .screenInit = f_platform_api_sdl___screenInit,
        .screenUninit = f_platform_api_sdl__screenUninit,
        .screenClear = f_platform_api_sdl__screenClear,
        #if F_CONFIG_LIB_SDL == 2
            .screenTextureGet = f_platform_api_sdl__screenTextureGet,
            .screenTextureSet = f_platform_api_sdl__screenTextureSet,
            .screenTextureSync = f_platform_api_sdl__screenTextureSync,
            #if F_CONFIG_SCREEN_RENDER_SDL2
                .screenToTexture = f_platform_api_sdl__screenToTexture,
                .screenClipSet = f_platform_api_sdl__screenClipSet,
            #endif
        #endif
        .screenShow = f_platform_api_sdl__screenShow,
        .screenPixelsGet = f_platform_api_sdl__screenPixelsGet,
        .screenSizeGet = f_platform_api_sdl__screenSizeGet,
        .screenVsyncGet = f_platform_api_sdl__screenVsyncGet,
        .screenZoomGet = f_platform_api_sdl__screenZoomGet,
        .screenZoomSet = f_platform_api_sdl__screenZoomSet,
        .screenFullscreenGet = f_platform_api_sdl__screenFullscreenGet,
        .screenFullscreenFlip = f_platform_api_sdl__screenFullscreenFlip,
    #elif F_CONFIG_SYSTEM_GAMEBUINO
        .screenInit = f_platform_api_gamebuino__screenInit,
        .screenPixelsGet = f_platform_api_gamebuino__screenPixelsGet,
        .screenSizeGet = f_platform_api_gamebuino__screenSizeGet,
        .screenVsyncGet = f_platform_api_gamebuino__screenVsyncGet,
    #elif F_CONFIG_SYSTEM_ODROID_GO
        .screenInit = f_platform_api_odroidgo__screenInit,
        .screenClear = f_platform_api_odroidgo__screenClear,
        .screenShow = f_platform_api_odroidgo__screenShow,
        .screenPixelsGet = f_platform_api_odroidgo__screenPixelsGet,
        .screenSizeGet = f_platform_api_odroidgo__screenSizeGet,
    #endif

    #if F_CONFIG_SCREEN_RENDER_SDL2
        .drawSetColor = f_platform_api_sdl__drawSetColor,
        .drawSetBlend = f_platform_api_sdl__drawSetBlend,
        .drawPixel = f_platform_api_sdl__drawPixel,
        .drawLine = f_platform_api_sdl__drawLine,
        .drawLineH = f_platform_api_sdl__drawLineH,
        .drawLineV = f_platform_api_sdl__drawLineV,
        .drawRectangleOutline = f_platform_api_sdl__drawRectangleOutline,
        .drawRectangleFilled = f_platform_api_sdl__drawRectangleFilled,
        .drawCircleOutline = f_platform_api_sdl__drawCircleOutline,
        .drawCircleFilled = f_platform_api_sdl__drawCircleFilled,
    #elif F_CONFIG_SCREEN_RENDER_SOFTWARE
        .drawPixel = f_platform_api_software__drawPixel,
        .drawLine = f_platform_api_software__drawLine,
        .drawLineH = f_platform_api_software__drawLineH,
        .drawLineV = f_platform_api_software__drawLineV,
        .drawRectangleOutline = f_platform_api_software__drawRectangleOutline,
        .drawRectangleFilled = f_platform_api_software__drawRectangleFilled,
        .drawCircleOutline = f_platform_api_software__drawCircleOutline,
        .drawCircleFilled = f_platform_api_software__drawCircleFilled,
    #endif

    #if F_CONFIG_SCREEN_RENDER_SDL2
        .textureSpriteToScreen = f_platform_api_sdl__textureSpriteToScreen,
        .textureNew = f_platform_api_sdl__textureNew,
        .textureDup = f_platform_api_sdl__textureDup,
        .textureFree = f_platform_api_sdl__textureFree,
        .textureBlit = f_platform_api_sdl__textureBlit,
        .textureBlitEx = f_platform_api_sdl__textureBlitEx,
    #elif F_CONFIG_SCREEN_RENDER_SOFTWARE
        .textureNew = f_platform_api_software__textureNew,
        .textureDup = f_platform_api_software__textureDup,
        .textureFree = f_platform_api_software__textureFree,
        .textureUpdate = f_platform_api_software__textureUpdate,
        .textureBlit = f_platform_api_software__textureBlit,
        .textureBlitEx = f_platform_api_software__textureBlitEx,
    #endif

    #if F_CONFIG_LIB_SDL
        .soundMuteGet = f_platform_api_sdl__soundMuteGet,
        .soundMuteFlip = f_platform_api_sdl__soundMuteFlip,
        .soundVolumeGetMax = f_platform_api_sdl__soundVolumeGetMax,
        .soundVolumeSet = f_platform_api_sdl__soundVolumeSet,
        .soundMusicNew = f_platform_api_sdl__soundMusicNew,
        .soundMusicFree = f_platform_api_sdl__soundMusicFree,
        .soundMusicPlay = f_platform_api_sdl__soundMusicPlay,
        .soundMusicStop = f_platform_api_sdl__soundMusicStop,
        .soundSampleNewFromFile = f_platform_api_sdl__soundSampleNewFromFile,
        .soundSampleNewFromData = f_platform_api_sdl__soundSampleNewFromData,
        .soundSampleFree = f_platform_api_sdl__soundSampleFree,
        .soundSamplePlay = f_platform_api_sdl__soundSamplePlay,
        .soundSampleStop = f_platform_api_sdl__soundSampleStop,
        .soundSampleIsPlaying = f_platform_api_sdl__soundSampleIsPlaying,
        .soundSampleChannelGet = f_platform_api_sdl__soundSampleChannelGet,
    #elif F_CONFIG_SYSTEM_GAMEBUINO
        .soundSamplePlay = f_platform_api_gamebuino__soundSamplePlay,
        .soundSampleStop = f_platform_api_gamebuino__soundSampleStop,
        .soundSampleIsPlaying = f_platform_api_gamebuino__soundSampleIsPlaying,
    #endif

    #if F_CONFIG_LIB_SDL
        .inputPoll = f_platform_api_sdl__inputPoll,
        #if F_CONFIG_TRAIT_KEYBOARD
            .inputKeyGet = f_platform_api_sdl__inputKeyGet,
        #endif
        .inputButtonGet = f_platform_api_sdl__inputButtonGet,
        .inputButtonPressGet = f_platform_api_sdl__inputButtonPressGet,
        .inputAnalogGet = f_platform_api_sdl__inputAnalogGet,
        .inputAnalogValueGet = f_platform_api_sdl__inputAnalogValueGet,
        .inputTouchCoordsGet = f_platform_api_sdl__inputTouchCoordsGet,
        .inputTouchDeltaGet = f_platform_api_sdl__inputTouchDeltaGet,
        .inputTouchTapGet = f_platform_api_sdl__inputTouchTapGet,
        .inputControllerClaim = f_platform_api_sdl__inputControllerClaim,
        .inputControllerRelease = f_platform_api_sdl__inputControllerRelease,
    #elif F_CONFIG_SYSTEM_GAMEBUINO
        .inputButtonGet = f_platform_api_gamebuino__inputButtonGet,
        .inputButtonPressGet = f_platform_api_gamebuino__inputButtonPressGet,
    #elif F_CONFIG_SYSTEM_ODROID_GO
        .inputPoll = f_platform_api_odroidgo__inputPoll,
        .inputButtonGet = f_platform_api_odroidgo__inputButtonGet,
        .inputButtonPressGet = f_platform_api_odroidgo__inputButtonPressGet,
    #endif

    #if F_CONFIG_LIB_STDLIB_FILES
        .dirCreate = f_platform_api_standard__dirCreate,
        .dirOpen = f_platform_api_standard__dirOpen,
        .fileStat = f_platform_api_standard__fileStat,
        .fileBufferRead = f_platform_api_standard__fileBufferRead,
        .fileBufferWrite = f_platform_api_standard__fileBufferWrite,
        .fileNew = f_platform_api_standard__fileNew,
        .fileFree = f_platform_api_standard__fileFree,
        .fileSeek = f_platform_api_standard__fileSeek,
        .fileRead = f_platform_api_standard__fileRead,
        .fileWrite = f_platform_api_standard__fileWrite,
        .fileWritef = f_platform_api_standard__fileWritef,
        .filePrint = f_platform_api_standard__filePrint,
        .fileFlush = f_platform_api_standard__fileFlush,
        .fileReadChar = f_platform_api_standard__fileReadChar,
        .fileReadCharUndo = f_platform_api_standard__fileReadCharUndo,
        #if F_CONFIG_SYSTEM_EMSCRIPTEN
            .fileSync = f_platform_api_emscripten__fileSync,
        #endif
    #elif F_CONFIG_SYSTEM_GAMEBUINO
        .fileStat = f_platform_api_gamebuino__fileStat,
        .fileBufferRead = f_platform_api_gamebuino__fileBufferRead,
        .fileBufferWrite = f_platform_api_gamebuino__fileBufferWrite,
        .fileNew = f_platform_api_gamebuino__fileNew,
        .fileFree = f_platform_api_gamebuino__fileFree,
        .fileRead = f_platform_api_gamebuino__fileRead,
        .fileWrite = f_platform_api_gamebuino__fileWrite,
        .filePrint = f_platform_api_gamebuino__filePrint,
    #elif F_CONFIG_SYSTEM_ODROID_GO
        .fileStat = f_platform_api_odroidgo__fileStat,
        .fileBufferRead = f_platform_api_odroidgo__fileBufferRead,
        .fileBufferWrite = f_platform_api_odroidgo__fileBufferWrite,
        .filePrint = f_platform_api_odroidgo__filePrint,
    #endif

    #if F_CONFIG_LIB_STDLIB_MEMORY
        .malloc = f_platform_api_standard__malloc,
        .mallocz = f_platform_api_standard__mallocz,
    #elif F_CONFIG_SYSTEM_ODROID_GO
        .malloc = f_platform_api_odroidgo__malloc,
        .mallocz = f_platform_api_odroidgo__mallocz,
    #endif
};

void f_platform_api__customExit(int Status)
{
    if(f__platform_api.customExit == NULL) {
        return;
    }

    f__platform_api.customExit(Status);
}

uint32_t f_platform_api__timeMsGet(void)
{
    if(f__platform_api.timeMsGet == NULL) {
        return 0;
    }

    return f__platform_api.timeMsGet();
}

void f_platform_api__timeMsWait(uint32_t Ms)
{
    if(f__platform_api.timeMsWait == NULL) {
        return;
    }

    f__platform_api.timeMsWait(Ms);
}

void f_platform_api__screenInit(void)
{
    if(f__platform_api.screenInit == NULL) {
        return;
    }

    f__platform_api.screenInit();
}

void f_platform_api__screenUninit(void)
{
    if(f__platform_api.screenUninit == NULL) {
        return;
    }

    f__platform_api.screenUninit();
}

void f_platform_api__screenClear(void)
{
    if(f__platform_api.screenClear == NULL) {
        return;
    }

    f__platform_api.screenClear();
}

FPlatformTextureScreen* f_platform_api__screenTextureGet(void)
{
    if(f__platform_api.screenTextureGet == NULL) {
        return NULL;
    }

    return f__platform_api.screenTextureGet();
}

void f_platform_api__screenTextureSet(FPlatformTextureScreen* Texture)
{
    if(f__platform_api.screenTextureSet == NULL) {
        return;
    }

    f__platform_api.screenTextureSet(Texture);
}

void f_platform_api__screenTextureSync(void)
{
    if(f__platform_api.screenTextureSync == NULL) {
        return;
    }

    f__platform_api.screenTextureSync();
}

void f_platform_api__screenToTexture(FPlatformTextureScreen* Texture, unsigned Frame)
{
    if(f__platform_api.screenToTexture == NULL) {
        return;
    }

    f__platform_api.screenToTexture(Texture, Frame);
}

void f_platform_api__screenClipSet(void)
{
    if(f__platform_api.screenClipSet == NULL) {
        return;
    }

    f__platform_api.screenClipSet();
}

void f_platform_api__screenShow(void)
{
    if(f__platform_api.screenShow == NULL) {
        return;
    }

    f__platform_api.screenShow();
}

FPixels* f_platform_api__screenPixelsGet(void)
{
    if(f__platform_api.screenPixelsGet == NULL) {
        return NULL;
    }

    return f__platform_api.screenPixelsGet();
}

FVecInt f_platform_api__screenSizeGet(void)
{
    if(f__platform_api.screenSizeGet == NULL) {
        return (FVecInt){0, 0};
    }

    return f__platform_api.screenSizeGet();
}

bool f_platform_api__screenVsyncGet(void)
{
    if(f__platform_api.screenVsyncGet == NULL) {
        return false;
    }

    return f__platform_api.screenVsyncGet();
}

int f_platform_api__screenZoomGet(void)
{
    if(f__platform_api.screenZoomGet == NULL) {
        return F_CONFIG_SCREEN_SIZE_ZOOM;
    }

    return f__platform_api.screenZoomGet();
}

void f_platform_api__screenZoomSet(int Zoom)
{
    if(f__platform_api.screenZoomSet == NULL) {
        return;
    }

    f__platform_api.screenZoomSet(Zoom);
}

bool f_platform_api__screenFullscreenGet(void)
{
    if(f__platform_api.screenFullscreenGet == NULL) {
        return F_CONFIG_SCREEN_FULLSCREEN;
    }

    return f__platform_api.screenFullscreenGet();
}

void f_platform_api__screenFullscreenFlip(void)
{
    if(f__platform_api.screenFullscreenFlip == NULL) {
        return;
    }

    f__platform_api.screenFullscreenFlip();
}

void f_platform_api__drawSetColor(void)
{
    if(f__platform_api.drawSetColor == NULL) {
        return;
    }

    f__platform_api.drawSetColor();
}

void f_platform_api__drawSetBlend(void)
{
    if(f__platform_api.drawSetBlend == NULL) {
        return;
    }

    f__platform_api.drawSetBlend();
}

void f_platform_api__drawPixel(int X, int Y)
{
    if(f__platform_api.drawPixel == NULL) {
        return;
    }

    f__platform_api.drawPixel(X, Y);
}

void f_platform_api__drawLine(int X1, int Y1, int X2, int Y2)
{
    if(f__platform_api.drawLine == NULL) {
        return;
    }

    f__platform_api.drawLine(X1, Y1, X2, Y2);
}

void f_platform_api__drawLineH(int X1, int X2, int Y)
{
    if(f__platform_api.drawLineH == NULL) {
        return;
    }

    f__platform_api.drawLineH(X1, X2, Y);
}

void f_platform_api__drawLineV(int X, int Y1, int Y2)
{
    if(f__platform_api.drawLineV == NULL) {
        return;
    }

    f__platform_api.drawLineV(X, Y1, Y2);
}

void f_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    if(f__platform_api.drawRectangleFilled == NULL) {
        return;
    }

    f__platform_api.drawRectangleFilled(X, Y, Width, Height);
}

void f_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    if(f__platform_api.drawRectangleOutline == NULL) {
        return;
    }

    f__platform_api.drawRectangleOutline(X, Y, Width, Height);
}

void f_platform_api__drawCircleOutline(int X, int Y, int Radius)
{
    if(f__platform_api.drawCircleOutline == NULL) {
        return;
    }

    f__platform_api.drawCircleOutline(X, Y, Radius);
}

void f_platform_api__drawCircleFilled(int X, int Y, int Radius)
{
    if(f__platform_api.drawCircleFilled == NULL) {
        return;
    }

    f__platform_api.drawCircleFilled(X, Y, Radius);
}

FPlatformTextureScreen* f_platform_api__textureSpriteToScreen(FPlatformTexture* SpriteTexture)
{
    if(f__platform_api.textureSpriteToScreen == NULL) {
        return NULL;
    }

    return f__platform_api.textureSpriteToScreen(SpriteTexture);
}

FPlatformTexture* f_platform_api__textureNew(const FPixels* Pixels)
{
    if(f__platform_api.textureNew == NULL) {
        return NULL;
    }

    return f__platform_api.textureNew(Pixels);
}

FPlatformTexture* f_platform_api__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels)
{
    if(f__platform_api.textureDup == NULL) {
        return NULL;
    }

    return f__platform_api.textureDup(Texture, Pixels);
}

void f_platform_api__textureFree(FPlatformTexture* Texture)
{
    if(f__platform_api.textureFree == NULL) {
        return;
    }

    f__platform_api.textureFree(Texture);
}

void f_platform_api__textureUpdate(FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame)
{
    if(f__platform_api.textureUpdate == NULL) {
        return;
    }

    f__platform_api.textureUpdate(Texture, Pixels, Frame);
}

void f_platform_api__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    if(f__platform_api.textureBlit == NULL) {
        return;
    }

    f__platform_api.textureBlit(Texture, Pixels, Frame, X, Y);
}

void f_platform_api__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    if(f__platform_api.textureBlitEx == NULL) {
        return;
    }

    f__platform_api.textureBlitEx(Texture, Pixels, Frame, X, Y, Scale, Angle, CenterX, CenterY);
}

bool f_platform_api__soundMuteGet(void)
{
    if(f__platform_api.soundMuteGet == NULL) {
        return false;
    }

    return f__platform_api.soundMuteGet();
}

void f_platform_api__soundMuteFlip(void)
{
    if(f__platform_api.soundMuteFlip == NULL) {
        return;
    }

    f__platform_api.soundMuteFlip();
}

int f_platform_api__soundVolumeGetMax(void)
{
    if(f__platform_api.soundVolumeGetMax == NULL) {
        return 0;
    }

    return f__platform_api.soundVolumeGetMax();
}

void f_platform_api__soundVolumeSet(int MusicVolume, int SamplesVolume)
{
    if(f__platform_api.soundVolumeSet == NULL) {
        return;
    }

    f__platform_api.soundVolumeSet(MusicVolume, SamplesVolume);
}

FPlatformMusic* f_platform_api__soundMusicNew(const char* Path)
{
    if(f__platform_api.soundMusicNew == NULL) {
        return NULL;
    }

    return f__platform_api.soundMusicNew(Path);
}

void f_platform_api__soundMusicFree(FPlatformMusic* Music)
{
    if(f__platform_api.soundMusicFree == NULL) {
        return;
    }

    f__platform_api.soundMusicFree(Music);
}

void f_platform_api__soundMusicPlay(FPlatformMusic* Music)
{
    if(f__platform_api.soundMusicPlay == NULL) {
        return;
    }

    f__platform_api.soundMusicPlay(Music);
}

void f_platform_api__soundMusicStop(void)
{
    if(f__platform_api.soundMusicStop == NULL) {
        return;
    }

    f__platform_api.soundMusicStop();
}

FPlatformSample* f_platform_api__soundSampleNewFromFile(const char* Path)
{
    if(f__platform_api.soundSampleNewFromFile == NULL) {
        return NULL;
    }

    return f__platform_api.soundSampleNewFromFile(Path);
}

FPlatformSample* f_platform_api__soundSampleNewFromData(const uint8_t* Data, size_t Size)
{
    if(f__platform_api.soundSampleNewFromData == NULL) {
        return NULL;
    }

    return f__platform_api.soundSampleNewFromData(Data, Size);
}

void f_platform_api__soundSampleFree(FPlatformSample* Sample)
{
    if(f__platform_api.soundSampleFree == NULL) {
        return;
    }

    f__platform_api.soundSampleFree(Sample);
}

void f_platform_api__soundSamplePlay(const FSample* Sample, int Channel, bool Loop)
{
    if(f__platform_api.soundSamplePlay == NULL) {
        return;
    }

    f__platform_api.soundSamplePlay(Sample, Channel, Loop);
}

void f_platform_api__soundSampleStop(int Channel)
{
    if(f__platform_api.soundSampleStop == NULL) {
        return;
    }

    f__platform_api.soundSampleStop(Channel);
}

bool f_platform_api__soundSampleIsPlaying(int Channel)
{
    if(f__platform_api.soundSampleIsPlaying == NULL) {
        return false;
    }

    return f__platform_api.soundSampleIsPlaying(Channel);
}

int f_platform_api__soundSampleChannelGet(void)
{
    if(f__platform_api.soundSampleChannelGet == NULL) {
        return -1;
    }

    return f__platform_api.soundSampleChannelGet();
}

void f_platform_api__inputPoll(void)
{
    if(f__platform_api.inputPoll == NULL) {
        return;
    }

    f__platform_api.inputPoll();
}

const FPlatformButton* f_platform_api__inputKeyGet(FKeyId Id)
{
    if(f__platform_api.inputKeyGet == NULL) {
        return NULL;
    }

    return f__platform_api.inputKeyGet(Id);
}

const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id)
{
    if(f__platform_api.inputButtonGet == NULL) {
        return NULL;
    }

    return f__platform_api.inputButtonGet(Controller, Id);
}

bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button)
{
    if(f__platform_api.inputButtonPressGet == NULL) {
        return false;
    }

    return f__platform_api.inputButtonPressGet(Button);
}

const FPlatformAnalog* f_platform_api__inputAnalogGet(const FPlatformController* Controller, FAnalogId Id)
{
    if(f__platform_api.inputAnalogGet == NULL) {
        return NULL;
    }

    return f__platform_api.inputAnalogGet(Controller, Id);
}

int f_platform_api__inputAnalogValueGet(const FPlatformAnalog* Analog)
{
    if(f__platform_api.inputAnalogValueGet == NULL) {
        return 0;
    }

    return f__platform_api.inputAnalogValueGet(Analog);
}

FVecInt f_platform_api__inputTouchCoordsGet(void)
{
    if(f__platform_api.inputTouchCoordsGet == NULL) {
        return (FVecInt){0, 0};
    }

    return f__platform_api.inputTouchCoordsGet();
}

FVecInt f_platform_api__inputTouchDeltaGet(void)
{
    if(f__platform_api.inputTouchDeltaGet == NULL) {
        return (FVecInt){0, 0};
    }

    return f__platform_api.inputTouchDeltaGet();
}

bool f_platform_api__inputTouchTapGet(void)
{
    if(f__platform_api.inputTouchTapGet == NULL) {
        return false;
    }

    return f__platform_api.inputTouchTapGet();
}

FPlatformController* f_platform_api__inputControllerClaim(FCallControllerBind* Callback)
{
    if(f__platform_api.inputControllerClaim == NULL) {
        return NULL;
    }

    return f__platform_api.inputControllerClaim(Callback);
}

void f_platform_api__inputControllerRelease(FPlatformController* Controller)
{
    if(f__platform_api.inputControllerRelease == NULL) {
        return;
    }

    f__platform_api.inputControllerRelease(Controller);
}

bool f_platform_api__dirCreate(const char* Path)
{
    if(f__platform_api.dirCreate == NULL) {
        return false;
    }

    return f__platform_api.dirCreate(Path);
}

FList* f_platform_api__dirOpen(FPath* Path)
{
    if(f__platform_api.dirOpen == NULL) {
        return NULL;
    }

    return f__platform_api.dirOpen(Path);
}

bool f_platform_api__fileStat(const char* Path, FPathInfo* Info)
{
    if(f__platform_api.fileStat == NULL) {
        return false;
    }

    return f__platform_api.fileStat(Path, Info);
}

bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size)
{
    if(f__platform_api.fileBufferRead == NULL) {
        return false;
    }

    return f__platform_api.fileBufferRead(Path, Buffer, Size);
}

bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size)
{
    if(f__platform_api.fileBufferWrite == NULL) {
        return false;
    }

    return f__platform_api.fileBufferWrite(Path, Buffer, Size);
}

FPlatformFile* f_platform_api__fileNew(const FPath* Path, unsigned Mode)
{
    if(f__platform_api.fileNew == NULL) {
        return NULL;
    }

    return f__platform_api.fileNew(Path, Mode);
}

void f_platform_api__fileFree(FPlatformFile* File)
{
    if(f__platform_api.fileFree == NULL) {
        return;
    }

    f__platform_api.fileFree(File);
}

bool f_platform_api__fileSeek(FPlatformFile* File, int Offset, FFileOffset Origin)
{
    if(f__platform_api.fileSeek == NULL) {
        return false;
    }

    return f__platform_api.fileSeek(File, Offset, Origin);
}

bool f_platform_api__fileRead(FPlatformFile* File, void* Buffer, size_t Size)
{
    if(f__platform_api.fileRead == NULL) {
        return false;
    }

    return f__platform_api.fileRead(File, Buffer, Size);
}

bool f_platform_api__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size)
{
    if(f__platform_api.fileWrite == NULL) {
        return false;
    }

    return f__platform_api.fileWrite(File, Buffer, Size);
}

bool f_platform_api__fileWritef(FPlatformFile* File, const char* Format, va_list Args)
{
    if(f__platform_api.fileWritef == NULL) {
        return false;
    }

    return f__platform_api.fileWritef(File, Format, Args);
}

void f_platform_api__filePrint(FPlatformFile* File, const char* String)
{
    if(f__platform_api.filePrint == NULL) {
        return;
    }

    f__platform_api.filePrint(File, String);
}

bool f_platform_api__fileFlush(FPlatformFile* File)
{
    if(f__platform_api.fileFlush == NULL) {
        return false;
    }

    return f__platform_api.fileFlush(File);
}

int f_platform_api__fileReadChar(FPlatformFile* File)
{
    if(f__platform_api.fileReadChar == NULL) {
        return -1;
    }

    return f__platform_api.fileReadChar(File);
}

int f_platform_api__fileReadCharUndo(FPlatformFile* File, int Char)
{
    if(f__platform_api.fileReadCharUndo == NULL) {
        return -1;
    }

    return f__platform_api.fileReadCharUndo(File, Char);
}

void f_platform_api__fileSync(void)
{
    if(f__platform_api.fileSync == NULL) {
        return;
    }

    f__platform_api.fileSync();
}


void* f_platform_api__malloc(size_t Size)
{
    if(f__platform_api.malloc == NULL) {
        return NULL;
    }

    return f__platform_api.malloc(Size);
}

void* f_platform_api__mallocz(size_t Size)
{
    if(f__platform_api.mallocz == NULL) {
        return NULL;
    }

    return f__platform_api.mallocz(Size);
}
