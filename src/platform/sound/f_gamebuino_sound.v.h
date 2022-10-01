/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_SOUND_GAMEBUINO_SOUND_V_H
#define F_INC_PLATFORM_SOUND_GAMEBUINO_SOUND_V_H

#include "f_gamebuino_sound.p.h"

#include "../f_platform.v.h"
#include "../../sound/f_sample.v.h"

extern FCallApi_SoundSamplePlay f_platform_api_gamebuino__soundSamplePlay;
extern FCallApi_SoundSampleStop f_platform_api_gamebuino__soundSampleStop;
extern FCallApi_SoundSampleIsPlaying f_platform_api_gamebuino__soundSampleIsPlaying;

#endif // F_INC_PLATFORM_SOUND_GAMEBUINO_SOUND_V_H
