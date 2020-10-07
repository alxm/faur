/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_PLATFORM_V_H
#define F_INC_PLATFORM_PLATFORM_V_H

#include "f_platform.p.h"

typedef struct FPlatformAnalog FPlatformAnalog;
typedef struct FPlatformButton FPlatformButton;
typedef struct FPlatformController FPlatformController;

typedef void FPlatformMusic;
typedef void FPlatformSample;

typedef void FPlatformTexture;
typedef void FPlatformTextureScreen;

typedef void FPlatformFile;

#include "../files/f_file.v.h"
#include "../files/f_path.v.h"
#include "../general/f_main.v.h"
#include "../graphics/f_pixels.v.h"
#include "../input/f_analog.v.h"
#include "../input/f_button.v.h"
#include "../math/f_fix.v.h"

extern const FPack f_pack__platform;

extern uint32_t f_platform_api__timeMsGet(void);
extern void f_platform_api__timeMsWait(uint32_t Ms);

extern void f_platform_api__screenInit(void);
extern void f_platform_api__screenUninit(void);
extern void f_platform_api__screenClear(void);
extern FPlatformTextureScreen* f_platform_api__screenTextureGet(void);
extern void f_platform_api__screenTextureSet(FPlatformTextureScreen* Texture);
extern void f_platform_api__screenTextureSync(void);
extern void f_platform_api__screenToTexture(FPlatformTextureScreen* Texture, unsigned Frame);
extern void f_platform_api__screenClipSet(void);
extern void f_platform_api__screenShow(void);
extern FPixels* f_platform_api__screenPixelsGet(void);
extern FVecInt f_platform_api__screenSizeGet(void);
extern bool f_platform_api__screenVsyncGet(void);
extern int f_platform_api__screenZoomGet(void);
extern void f_platform_api__screenZoomSet(int Zoom);
extern bool f_platform_api__screenFullscreenGet(void);
extern void f_platform_api__screenFullscreenFlip(void);

extern void f_platform_api__drawSetColor(void);
extern void f_platform_api__drawSetBlend(void);
extern void f_platform_api__drawPixel(int X, int Y);
extern void f_platform_api__drawLine(int X1, int Y1, int X2, int Y2);
extern void f_platform_api__drawLineH(int X1, int X2, int Y);
extern void f_platform_api__drawLineV(int X, int Y1, int Y2);
extern void f_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height);
extern void f_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height);
extern void f_platform_api__drawCircleOutline(int X, int Y, int Radius);
extern void f_platform_api__drawCircleFilled(int X, int Y, int Radius);

extern FPlatformTextureScreen* f_platform_api__textureSpriteToScreen(FPlatformTexture* SpriteTexture);
extern FPlatformTexture* f_platform_api__textureNew(const FPixels* Pixels);
extern FPlatformTexture* f_platform_api__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels);
extern void f_platform_api__textureFree(FPlatformTexture* Texture);
extern void f_platform_api__textureUpdate(FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame);
extern void f_platform_api__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
extern void f_platform_api__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);

extern bool f_platform_api__soundMuteGet(void);
extern void f_platform_api__soundMuteFlip(void);
extern int f_platform_api__soundVolumeGetMax(void);
extern void f_platform_api__soundVolumeSet(int MusicVolume, int SamplesVolume);

extern FPlatformMusic* f_platform_api__soundMusicNew(const char* Path);
extern void f_platform_api__soundMusicFree(FPlatformMusic* Music);
extern void f_platform_api__soundMusicPlay(FPlatformMusic* Music);
extern void f_platform_api__soundMusicStop(void);

extern FPlatformSample* f_platform_api__soundSampleNewFromFile(const char* Path);
extern FPlatformSample* f_platform_api__soundSampleNewFromData(const uint8_t* Data, size_t Size);
extern void f_platform_api__soundSampleFree(FPlatformSample* Sample);
extern void f_platform_api__soundSamplePlay(const FPlatformSample* Sample, int Channel, bool Loop);
extern void f_platform_api__soundSampleStop(int Channel);
extern bool f_platform_api__soundSampleIsPlaying(int Channel);
extern int f_platform_api__soundSampleChannelGet(void);

extern void f_platform_api__inputPoll(void);

extern const FPlatformButton* f_platform_api__inputKeyGet(FKeyId Id);
extern const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id);
extern bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button);

extern const FPlatformAnalog* f_platform_api__inputAnalogGet(const FPlatformController* Controller, FAnalogId Id);
extern int f_platform_api__inputAnalogValueGet(const FPlatformAnalog* Analog);

extern FVecInt f_platform_api__inputTouchCoordsGet(void);
extern FVecInt f_platform_api__inputTouchDeltaGet(void);
extern bool f_platform_api__inputTouchTapGet(void);

extern FPlatformController* f_platform_api__inputControllerClaim(FControllerBind* Callback);
extern void f_platform_api__inputControllerRelease(FPlatformController* Controller);

extern bool f_platform_api__fileStat(const char* Path, FPathInfo* Info);
extern bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size);
extern bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size);
extern FPlatformFile* f_platform_api__fileNew(const FPath* Path, FFileMode Mode);
extern void f_platform_api__fileFree(FPlatformFile* File);
extern bool f_platform_api__fileSeek(FPlatformFile* File, int Offset, FFileOffset Origin);
extern bool f_platform_api__fileRead(FPlatformFile* File, void* Buffer, size_t Size);
extern bool f_platform_api__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size);
extern bool f_platform_api__fileWritef(FPlatformFile* File, const char* Format, va_list Args);
extern void f_platform_api__filePrint(FPlatformFile* File, const char* String);
extern bool f_platform_api__fileFlush(FPlatformFile* File);
extern int f_platform_api__fileReadChar(FPlatformFile* File);
extern int f_platform_api__fileReadCharUndo(FPlatformFile* File, int Char);
extern void f_platform_api__fileSync(void);

#endif // F_INC_PLATFORM_PLATFORM_V_H
