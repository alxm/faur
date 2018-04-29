/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#pragma once

#include "a2x_pack_platform.p.h"

typedef struct APlatformTexture APlatformTexture;
typedef struct APlatformSfx APlatformSfx;
typedef void APlatformMusic;

#include "a2x_pack_sprite.v.h"

extern void a_platform__init(void);
extern void a_platform__init2(void);
extern void a_platform__uninit(void);

extern uint32_t a_platform__getMs(void);

extern void a_platform__setScreen(int Width, int Height, bool FullScreen);
extern void a_platform__showScreen(void);
extern void a_platform__setFullScreen(bool FullScreen);
extern void a_platform__getNativeResolution(int* Width, int* Height);

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

extern APlatformTexture* a_platform__newScreenTexture(int Width, int Height);
extern void a_platform__commitSpriteTexture(ASprite* Sprite);
extern void a_platform__freeTexture(APlatformTexture* Texture);
extern void a_platform__blitTexture(APlatformTexture* Texture, int X, int Y, bool FillFlat);
extern void a_platform__blitTextureEx(APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat);

extern void a_platform__setRenderTarget(APlatformTexture* Texture);
extern void a_platform__getTargetPixels(APixel* Pixels, int Width);
extern void a_platform__setTargetClip(int X, int Y, int Width, int Height);

extern int a_platform__getMaxVolome(void);

extern APlatformMusic* a_platform__newMusic(const char* Path);
extern void a_platform__freeMusic(APlatformMusic* Music);
extern void a_platform__setMusicVolume(int Volume);
extern void a_platform__playMusic(APlatformMusic* Music);
extern void a_platform__stopMusic(void);
extern void a_platform__toggleMusic(void);

extern APlatformSfx* a_platform__newSfxFromFile(const char* Path);
extern APlatformSfx* a_platform__newSfxFromData(const uint8_t* Data, int Size);
extern void a_platform__freeSfx(APlatformSfx* Sfx);
extern void a_platform__setSfxVolume(APlatformSfx* Sfx, int Volume);
extern void a_platform__setSfxVolumeAll(int Volume);
extern void a_platform__playSfx(APlatformSfx* Sfx, bool Loop, bool OwnChannel);
extern void a_platform__stopSfx(APlatformSfx* Sfx);
extern bool a_platform__isSfxPlaying(APlatformSfx* Sfx);

extern void a_platform__bindInputs(void);
extern void a_platform__pollInputs(void);
