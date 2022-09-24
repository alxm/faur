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

FPlatformApi f__platform_api;

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
        return 0;
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
        return false;
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
