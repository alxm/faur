/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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
#include "../sound/f_sample.v.h"

typedef void FCallApi_CustomExit(int Status);

typedef uint32_t FCallApi_TimeMsGet(void);
typedef void FCallApi_TimeMsWait(uint32_t Ms);

typedef void FCallApi_ScreenInit(void);
typedef void FCallApi_ScreenUninit(void);
typedef void FCallApi_ScreenClear(void);
typedef void FCallApi_ScreenShow(void);
typedef FPixels* FCallApi_ScreenPixelsGet(void);
typedef FVecInt FCallApi_ScreenSizeGet(void);
typedef bool FCallApi_ScreenVsyncGet(void);
typedef int FCallApi_ScreenZoomGet(void);
typedef void FCallApi_ScreenZoomSet(int Zoom);
typedef bool FCallApi_ScreenFullscreenGet(void);
typedef void FCallApi_ScreenFullscreenFlip(void);
typedef FPlatformTextureScreen* FCallApi_ScreenTextureGet(void);
typedef void FCallApi_ScreenTextureSet(FPlatformTextureScreen* Texture);
typedef void FCallApi_ScreenTextureSync(void);
typedef void FCallApi_ScreenToTexture(FPlatformTextureScreen* Texture, unsigned Frame);
typedef void FCallApi_ScreenClipSet(void);

typedef void FCallApi_DrawPixel(int X, int Y);
typedef void FCallApi_DrawLine(int X1, int Y1, int X2, int Y2);
typedef void FCallApi_DrawLineH(int X1, int X2, int Y);
typedef void FCallApi_DrawLineV(int X, int Y1, int Y2);
typedef void FCallApi_DrawRectangleFilled(int X, int Y, int Width, int Height);
typedef void FCallApi_DrawRectangleOutline(int X, int Y, int Width, int Height);
typedef void FCallApi_DrawCircleOutline(int X, int Y, int Radius);
typedef void FCallApi_DrawCircleFilled(int X, int Y, int Radius);
typedef void FCallApi_DrawSetColor(void);
typedef void FCallApi_DrawSetBlend(void);

typedef FPlatformTexture* FCallApi_TextureNew(const FPixels* Pixels);
typedef FPlatformTexture* FCallApi_TextureDup(const FPlatformTexture* Texture, const FPixels* Pixels);
typedef void FCallApi_TextureFree(FPlatformTexture* Texture);
typedef void FCallApi_TextureUpdate(FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame);
typedef void FCallApi_TextureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
typedef void FCallApi_TextureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);
typedef FPlatformTextureScreen* FCallApi_TextureSpriteToScreen(FPlatformTexture* SpriteTexture);

typedef FPixels* FCallApi_ImageRead(const char* Path);
typedef void FCallApi_ImageWrite(const char* Path, const FPixels* Pixels, unsigned Frame, char* Title, char* Description);

typedef bool FCallApi_SoundMuteGet(void);
typedef void FCallApi_SoundMuteFlip(void);
typedef int FCallApi_SoundVolumeGetMax(void);
typedef void FCallApi_SoundVolumeSet(int MusicVolume, int SamplesVolume);
typedef FPlatformMusic* FCallApi_SoundMusicNew(const char* Path);
typedef void FCallApi_SoundMusicFree(FPlatformMusic* Music);
typedef void FCallApi_SoundMusicPlay(FPlatformMusic* Music);
typedef void FCallApi_SoundMusicStop(void);
typedef FPlatformSample* FCallApi_SoundSampleNewFromFile(const char* Path);
typedef FPlatformSample* FCallApi_SoundSampleNewFromData(const uint8_t* Data, size_t Size);
typedef void FCallApi_SoundSampleFree(FPlatformSample* Sample);
typedef void FCallApi_SoundSamplePlay(const FSample* Sample, int Channel, bool Loop);
typedef void FCallApi_SoundSampleStop(int Channel);
typedef bool FCallApi_SoundSampleIsPlaying(int Channel);
typedef int FCallApi_SoundSampleChannelGet(void);

typedef void FCallApi_InputPoll(void);
typedef const FPlatformButton* FCallApi_InputKeyGet(FKeyId Id);
typedef const FPlatformButton* FCallApi_InputButtonGet(const FPlatformController* Controller, FButtonId Id);
typedef bool FCallApi_InputButtonPressGet(const FPlatformButton* Button);
typedef const FPlatformAnalog* FCallApi_InputAnalogGet(const FPlatformController* Controller, FAnalogId Id);
typedef int FCallApi_InputAnalogValueGet(const FPlatformAnalog* Analog);
typedef FVecInt FCallApi_InputTouchCoordsGet(void);
typedef FVecInt FCallApi_InputTouchDeltaGet(void);
typedef bool FCallApi_InputTouchTapGet(void);
typedef FPlatformController* FCallApi_InputControllerClaim(FCallControllerBind* Callback);
typedef void FCallApi_InputControllerRelease(FPlatformController* Controller);

typedef bool FCallApi_DirCreate(const char* Path);
typedef FList* FCallApi_DirOpen(FPath* Path);
typedef bool FCallApi_FileStat(const char* Path, FPathInfo* Info);
typedef bool FCallApi_FileBufferRead(const char* Path, void* Buffer, size_t Size);
typedef bool FCallApi_FileBufferWrite(const char* Path, const void* Buffer, size_t Size);
typedef FPlatformFile* FCallApi_FileNew(const FPath* Path, unsigned Mode);
typedef void FCallApi_FileFree(FPlatformFile* File);
typedef bool FCallApi_FileSeek(FPlatformFile* File, int Offset, FFileOffset Origin);
typedef bool FCallApi_FileRead(FPlatformFile* File, void* Buffer, size_t Size);
typedef bool FCallApi_FileWrite(FPlatformFile* File, const void* Buffer, size_t Size);
typedef bool FCallApi_FileWritef(FPlatformFile* File, const char* Format, va_list Args);
typedef void FCallApi_FilePrint(FPlatformFile* File, const char* String);
typedef bool FCallApi_FileFlush(FPlatformFile* File);
typedef int FCallApi_FileReadChar(FPlatformFile* File);
typedef int FCallApi_FileReadCharUndo(FPlatformFile* File, int Char);
typedef void FCallApi_FileSync(void);

typedef void* FCallApi_Malloc(size_t Size);
typedef void* FCallApi_Mallocz(size_t Size);

typedef struct FPlatformApi {
    FCallApi_CustomExit* customExit;

    FCallApi_TimeMsGet* timeMsGet;
    FCallApi_TimeMsWait* timeMsWait;

    FCallApi_ScreenInit* screenInit;
    FCallApi_ScreenUninit* screenUninit;
    FCallApi_ScreenClear* screenClear;
    FCallApi_ScreenShow* screenShow;
    FCallApi_ScreenPixelsGet* screenPixelsGet;
    FCallApi_ScreenSizeGet* screenSizeGet;
    FCallApi_ScreenVsyncGet* screenVsyncGet;
    FCallApi_ScreenZoomGet* screenZoomGet;
    FCallApi_ScreenZoomSet* screenZoomSet;
    FCallApi_ScreenFullscreenGet* screenFullscreenGet;
    FCallApi_ScreenFullscreenFlip* screenFullscreenFlip;
    FCallApi_ScreenTextureGet* screenTextureGet;
    FCallApi_ScreenTextureSet* screenTextureSet;
    FCallApi_ScreenTextureSync* screenTextureSync;
    FCallApi_ScreenToTexture* screenToTexture;
    FCallApi_ScreenClipSet* screenClipSet;

    FCallApi_DrawPixel* drawPixel;
    FCallApi_DrawLine* drawLine;
    FCallApi_DrawLineH* drawLineH;
    FCallApi_DrawLineV* drawLineV;
    FCallApi_DrawRectangleOutline* drawRectangleOutline;
    FCallApi_DrawRectangleFilled* drawRectangleFilled;
    FCallApi_DrawCircleOutline* drawCircleOutline;
    FCallApi_DrawCircleFilled* drawCircleFilled;
    FCallApi_DrawSetColor* drawSetColor;
    FCallApi_DrawSetBlend* drawSetBlend;

    FCallApi_TextureNew* textureNew;
    FCallApi_TextureDup* textureDup;
    FCallApi_TextureFree* textureFree;
    FCallApi_TextureUpdate* textureUpdate;
    FCallApi_TextureBlit* textureBlit;
    FCallApi_TextureBlitEx* textureBlitEx;
    FCallApi_TextureSpriteToScreen* textureSpriteToScreen;

    FCallApi_ImageRead* imageRead;
    FCallApi_ImageWrite* imageWrite;

    FCallApi_SoundMuteGet* soundMuteGet;
    FCallApi_SoundMuteFlip* soundMuteFlip;
    FCallApi_SoundVolumeGetMax* soundVolumeGetMax;
    FCallApi_SoundVolumeSet* soundVolumeSet;

    FCallApi_SoundMusicNew* soundMusicNew;
    FCallApi_SoundMusicFree* soundMusicFree;
    FCallApi_SoundMusicPlay* soundMusicPlay;
    FCallApi_SoundMusicStop* soundMusicStop;

    FCallApi_SoundSampleNewFromFile* soundSampleNewFromFile;
    FCallApi_SoundSampleNewFromData* soundSampleNewFromData;
    FCallApi_SoundSampleFree* soundSampleFree;
    FCallApi_SoundSamplePlay* soundSamplePlay;
    FCallApi_SoundSampleStop* soundSampleStop;
    FCallApi_SoundSampleIsPlaying* soundSampleIsPlaying;
    FCallApi_SoundSampleChannelGet* soundSampleChannelGet;

    FCallApi_InputPoll* inputPoll;

    FCallApi_InputKeyGet* inputKeyGet;
    FCallApi_InputButtonGet* inputButtonGet;
    FCallApi_InputButtonPressGet* inputButtonPressGet;

    FCallApi_InputAnalogGet* inputAnalogGet;
    FCallApi_InputAnalogValueGet* inputAnalogValueGet;

    FCallApi_InputTouchCoordsGet* inputTouchCoordsGet;
    FCallApi_InputTouchDeltaGet* inputTouchDeltaGet;
    FCallApi_InputTouchTapGet* inputTouchTapGet;

    FCallApi_InputControllerClaim* inputControllerClaim;
    FCallApi_InputControllerRelease* inputControllerRelease;

    FCallApi_DirCreate* dirCreate;
    FCallApi_DirOpen* dirOpen;

    FCallApi_FileStat* fileStat;
    FCallApi_FileBufferRead* fileBufferRead;
    FCallApi_FileBufferWrite* fileBufferWrite;
    FCallApi_FileNew* fileNew;
    FCallApi_FileFree* fileFree;
    FCallApi_FileSeek* fileSeek;
    FCallApi_FileRead* fileRead;
    FCallApi_FileWrite* fileWrite;
    FCallApi_FileWritef* fileWritef;
    FCallApi_FilePrint* filePrint;
    FCallApi_FileFlush* fileFlush;
    FCallApi_FileReadChar* fileReadChar;
    FCallApi_FileReadCharUndo* fileReadCharUndo;
    FCallApi_FileSync* fileSync;

    FCallApi_Malloc* malloc;
    FCallApi_Mallocz* mallocz;
} FPlatformApi;

extern const FPack f_pack__platform;

extern void f_platform_api__customExit(int Status);

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

extern FPixels* f_platform_api__imageRead(const char* Path);
extern void f_platform_api__imageWrite(const char* Path, const FPixels* Pixels, unsigned Frame, char* Title, char* Description);

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
extern void f_platform_api__soundSamplePlay(const FSample* Sample, int Channel, bool Loop);
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

extern FPlatformController* f_platform_api__inputControllerClaim(FCallControllerBind* Callback);
extern void f_platform_api__inputControllerRelease(FPlatformController* Controller);

extern bool f_platform_api__dirCreate(const char* Path);
extern FList* f_platform_api__dirOpen(FPath* Path);
extern bool f_platform_api__fileStat(const char* Path, FPathInfo* Info);
extern bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size);
extern bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size);
extern FPlatformFile* f_platform_api__fileNew(const FPath* Path, unsigned Mode);
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

extern void* f_platform_api__malloc(size_t Size);
extern void* f_platform_api__mallocz(size_t Size);

#endif // F_INC_PLATFORM_PLATFORM_V_H
