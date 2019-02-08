/*
    Copyright 2010, 2016-2018 Alex Margarit
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

#pragma once

#include "a2x_pack_platform.p.h"

typedef struct APlatformController APlatformController;
typedef struct APlatformButton APlatformButton;
typedef struct APlatformAnalog APlatformAnalog;
typedef struct APlatformTouch APlatformTouch;

typedef struct APlatformTexture APlatformTexture;

typedef void APlatformSample;
typedef void APlatformMusic;

#include "a2x_pack_input_analog.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_sprite.v.h"

extern void a_platform__init(void);
extern void a_platform__init2(void);
extern void a_platform__uninit(void);

extern uint32_t a_platform__msGet(void);
extern void a_platform__msWait(uint32_t Ms);

extern void a_platform__screenInit(void);
extern void a_platform__screenShow(void);
extern void a_platform__screenResolutionGetNative(int* Width, int* Height);

extern void a_platform__renderClear(void);
extern void a_platform__renderSetDrawColor(void);
extern void a_platform__renderSetBlendMode(void);

extern void a_platform__drawPixel(int X, int Y);
extern void a_platform__drawLine(int X1, int Y1, int X2, int Y2);
extern void a_platform__drawHLine(int X1, int X2, int Y);
extern void a_platform__drawVLine(int X, int Y1, int Y2);
extern void a_platform__drawRectangleFilled(int X, int Y, int Width, int Height);
extern void a_platform__drawRectangleOutline(int X, int Y, int Width, int Height);
extern void a_platform__drawCircleOutline(int X, int Y, int Radius);
extern void a_platform__drawCircleFilled(int X, int Y, int Radius);

extern APlatformTexture* a_platform__textureScreenNew(int Width, int Height);
extern APlatformTexture* a_platform__textureSpriteNew(const ASprite* Sprite);
extern void a_platform__textureFree(APlatformTexture* Texture);
extern void a_platform__textureBlit(const APlatformTexture* Texture, int X, int Y, bool FillFlat);
extern void a_platform__textureBlitEx(const APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat);

extern void a_platform__renderTargetSet(APlatformTexture* Texture);
extern void a_platform__renderTargetPixelsGet(APixel* Pixels, int Width);
extern void a_platform__renderTargetClipSet(int X, int Y, int Width, int Height);

extern int a_platform__volumeGetMax(void);

extern APlatformMusic* a_platform__musicNew(const char* Path);
extern void a_platform__musicFree(APlatformMusic* Music);
extern void a_platform__musicVolumeSet(int Volume);
extern void a_platform__musicPlay(APlatformMusic* Music);
extern void a_platform__musicStop(void);

extern APlatformSample* a_platform__sampleNewFromFile(const char* Path);
extern APlatformSample* a_platform__sampleNewFromData(const uint8_t* Data, int Size);
extern void a_platform__sampleFree(APlatformSample* Sample);
extern void a_platform__sampleVolumeSet(APlatformSample* Sample, int Volume);
extern void a_platform__sampleVolumeSetAll(int Volume);
extern void a_platform__samplePlay(APlatformSample* Sample, int Channel, bool Loop);
extern void a_platform__sampleStop(int Channel);
extern bool a_platform__sampleIsPlaying(int Channel);
extern int a_platform__sampleChannelGet(void);

extern void a_platform__inputsPoll(void);

extern APlatformButton* a_platform__buttonGet(int Id);
extern const char* a_platform__buttonNameGet(const APlatformButton* Button);
extern bool a_platform__buttonPressGet(const APlatformButton* Button);
extern void a_platform__buttonForward(int Source, int Destination);

extern APlatformAnalog* a_platform__analogGet(int Id);
extern const char* a_platform__analogNameGet(const APlatformAnalog* Analog);
extern int a_platform__analogValueGet(const APlatformAnalog* Analog);
extern void a_platform__analogForward(AAxisId Source, AButtonId Negative, AButtonId Positive);

extern APlatformTouch* a_platform__touchGet(void);
extern void a_platform__touchCoordsGet(const APlatformTouch* Touch, int* X, int* Y);
extern void a_platform__touchDeltaGet(const APlatformTouch* Touch, int* Dx, int* Dy);
extern bool a_platform__touchTapGet(const APlatformTouch* Touch);

extern unsigned a_platform__controllerNumGet(void);
extern void a_platform__controllerSet(unsigned Index);
extern bool a_platform__controllerIsMapped(void);
