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

#ifndef F_INC_PLATFORM_SOUND_SDL_SOUND_V_H
#define F_INC_PLATFORM_SOUND_SDL_SOUND_V_H

#include "f_sdl_sound.p.h"

#include "../f_platform.v.h"

extern void f_platform_sdl_sound__init(void);
extern void f_platform_sdl_sound__uninit(void);

extern bool f_platform_api_sdl__soundMuteGet(void);
extern void f_platform_api_sdl__soundMuteFlip(void);
extern int f_platform_api_sdl__soundVolumeGetMax(void);
extern void f_platform_api_sdl__soundVolumeSet(int MusicVolume, int SamplesVolume);

extern FPlatformMusic* f_platform_api_sdl__soundMusicNew(const char* Path);
extern void f_platform_api_sdl__soundMusicFree(FPlatformMusic* Music);
extern void f_platform_api_sdl__soundMusicPlay(FPlatformMusic* Music);
extern void f_platform_api_sdl__soundMusicStop(void);

extern FPlatformSample* f_platform_api_sdl__soundSampleNewFromFile(const char* Path);
extern FPlatformSample* f_platform_api_sdl__soundSampleNewFromData(const uint8_t* Data, size_t Size);
extern void f_platform_api_sdl__soundSampleFree(FPlatformSample* Sample);

extern void f_platform_api_sdl__soundSamplePlay(const FSample* Sample, int Channel, bool Loop);
extern void f_platform_api_sdl__soundSampleStop(int Channel);
extern bool f_platform_api_sdl__soundSampleIsPlaying(int Channel);
extern int f_platform_api_sdl__soundSampleChannelGet(void);

#endif // F_INC_PLATFORM_SOUND_SDL_SOUND_V_H
