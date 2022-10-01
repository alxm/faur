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

extern FCallApi_SoundMuteGet f_platform_api_sdl__soundMuteGet;
extern FCallApi_SoundMuteFlip f_platform_api_sdl__soundMuteFlip;
extern FCallApi_SoundVolumeGetMax f_platform_api_sdl__soundVolumeGetMax;
extern FCallApi_SoundVolumeSet f_platform_api_sdl__soundVolumeSet;

extern FCallApi_SoundMusicNew f_platform_api_sdl__soundMusicNew;
extern FCallApi_SoundMusicFree f_platform_api_sdl__soundMusicFree;
extern FCallApi_SoundMusicPlay f_platform_api_sdl__soundMusicPlay;
extern FCallApi_SoundMusicStop f_platform_api_sdl__soundMusicStop;

extern FCallApi_SoundSampleNewFromFile f_platform_api_sdl__soundSampleNewFromFile;
extern FCallApi_SoundSampleNewFromData f_platform_api_sdl__soundSampleNewFromData;
extern FCallApi_SoundSampleFree f_platform_api_sdl__soundSampleFree;

extern FCallApi_SoundSamplePlay f_platform_api_sdl__soundSamplePlay;
extern FCallApi_SoundSampleStop f_platform_api_sdl__soundSampleStop;
extern FCallApi_SoundSampleIsPlaying f_platform_api_sdl__soundSampleIsPlaying;
extern FCallApi_SoundSampleChannelGet f_platform_api_sdl__soundSampleChannelGet;

#endif // F_INC_PLATFORM_SOUND_SDL_SOUND_V_H
