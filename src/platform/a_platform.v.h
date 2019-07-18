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

#ifndef A_INC_PLATFORM_PLATFORM_V_H
#define A_INC_PLATFORM_PLATFORM_V_H

#include "platform/a_platform.p.h"

typedef struct APlatformInputAnalog APlatformInputAnalog;
typedef struct APlatformInputButton APlatformInputButton;
typedef struct APlatformInputController APlatformInputController;
typedef struct APlatformInputTouch APlatformInputTouch;

typedef void APlatformSoundMusic;
typedef void APlatformSoundSample;

typedef struct APlatformTexture APlatformTexture;

#include "general/a_main.v.h"
#include "inputs/a_analog.v.h"
#include "inputs/a_button.v.h"
#include "math/a_fix.v.h"

extern const APack a_pack__platform;

extern uint32_t a_platform_api__timeMsGet(void);
extern void a_platform_api__timeMsWait(uint32_t Ms);

extern void a_platform_api__screenInit(void);
extern void a_platform_api__screenUninit(void);
extern void a_platform_api__screenClear(void);
extern APlatformTexture* a_platform_api__screenTextureGet(void);
extern void a_platform_api__screenTextureSet(APlatformTexture* Texture);
extern void a_platform_api__screenTextureRead(APixels* Pixels, unsigned Frame);
extern void a_platform_api__screenClipSet(int X, int Y, int Width, int Height);
extern void a_platform_api__screenDraw(void);
extern void a_platform_api__screenShow(void);
extern APixels* a_platform_api__screenPixelsGet(void);
extern AVectorInt a_platform_api__screenSizeGet(void);
extern bool a_platform_api__screenVsyncGet(void);
extern int a_platform_api__screenZoomGet(void);
extern void a_platform_api__screenZoomSet(int Zoom);
extern bool a_platform_api__screenFullscreenGet(void);
extern void a_platform_api__screenFullscreenFlip(void);
extern void a_platform_api__screenMouseCursorSet(bool Show);

extern void a_platform_api__renderSetDrawColor(void);
extern void a_platform_api__renderSetBlendMode(void);

extern void a_platform_api__drawPixel(int X, int Y);
extern void a_platform_api__drawLine(int X1, int Y1, int X2, int Y2);
extern void a_platform_api__drawHLine(int X1, int X2, int Y);
extern void a_platform_api__drawVLine(int X, int Y1, int Y2);
extern void a_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height);
extern void a_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height);
extern void a_platform_api__drawCircleOutline(int X, int Y, int Radius);
extern void a_platform_api__drawCircleFilled(int X, int Y, int Radius);

extern APlatformTexture* a_platform_api__textureNew(const APixels* Pixels, unsigned Frame);
extern void a_platform_api__textureFree(APlatformTexture* Texture);
extern void a_platform_api__textureBlit(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y);
extern void a_platform_api__textureBlitEx(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY);

extern bool a_platform_api__soundMuteGet(void);
extern void a_platform_api__soundMuteFlip(void);
extern int a_platform_api__soundVolumeGetMax(void);

extern APlatformSoundMusic* a_platform_api__soundMusicNew(const char* Path);
extern void a_platform_api__soundMusicFree(APlatformSoundMusic* Music);
extern void a_platform_api__soundMusicVolumeSet(int Volume);
extern void a_platform_api__soundMusicPlay(APlatformSoundMusic* Music);
extern void a_platform_api__soundMusicStop(void);

extern APlatformSoundSample* a_platform_api__soundSampleNewFromFile(const char* Path);
extern APlatformSoundSample* a_platform_api__soundSampleNewFromData(const uint8_t* Data, int Size);
extern void a_platform_api__soundSampleFree(APlatformSoundSample* Sample);
extern void a_platform_api__soundSampleVolumeSet(APlatformSoundSample* Sample, int Volume);
extern void a_platform_api__soundSampleVolumeSetAll(int Volume);
extern void a_platform_api__soundSamplePlay(APlatformSoundSample* Sample, int Channel, bool Loop);
extern void a_platform_api__soundSampleStop(int Channel);
extern bool a_platform_api__soundSampleIsPlaying(int Channel);
extern int a_platform_api__soundSampleChannelGet(void);

extern void a_platform_api__inputPoll(void);

extern APlatformInputButton* a_platform_api__inputButtonGet(int Id);
extern const char* a_platform_api__inputButtonNameGet(const APlatformInputButton* Button);
extern bool a_platform_api__inputButtonPressGet(const APlatformInputButton* Button);
extern void a_platform_api__inputButtonForward(int Source, int Destination);

extern APlatformInputAnalog* a_platform_api__inputAnalogGet(int Id);
extern const char* a_platform_api__inputAnalogNameGet(const APlatformInputAnalog* Analog);
extern int a_platform_api__inputAnalogValueGet(const APlatformInputAnalog* Analog);
extern void a_platform_api__inputAnalogForward(AAxisId Source, AButtonId Negative, AButtonId Positive);

extern APlatformInputTouch* a_platform_api__inputTouchGet(void);
extern AVectorInt a_platform_api__inputTouchCoordsGet(const APlatformInputTouch* Touch);
extern AVectorInt a_platform_api__inputTouchDeltaGet(const APlatformInputTouch* Touch);
extern bool a_platform_api__inputTouchTapGet(const APlatformInputTouch* Touch);

extern unsigned a_platform_api__inputControllerNumGet(void);
extern void a_platform_api__inputControllerSet(unsigned Index);
extern bool a_platform_api__inputControllerIsMapped(void);

#endif // A_INC_PLATFORM_PLATFORM_V_H
