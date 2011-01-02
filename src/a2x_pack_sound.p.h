/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_SOUND_PH
#define A2X_PACK_SOUND_PH

#include "a2x_app_includes.h"

#define A_VOLUME_STEP 1
#define A_MILIS_VOLUME (1000 / 2)

#define A_MAX_VOLUME MIX_MAX_VOLUME

typedef Mix_Music Music;
typedef Mix_Chunk Sound;

extern int a__volume;
extern int a__volumeAdjust;

extern Music* a_music_load(const char* const path);
extern void a_music_free(Music* const m);
extern void a_music_play(Music* const m);
extern void a_music_stop(void);

extern Sound* a_sfx_fromFile(const char* const path);
#define a_sfx_fromData(s) a_sfx__fromData(s, sizeof(s))
extern Sound* a_sfx__fromData(const uint16_t* const data, const int size);
extern void a_sfx_free(Sound* const s);
extern void a_sfx_play(Sound* const s);

extern void a_sfx_volume(const int v);
extern void a_sound_adjustVolume(void);

#endif // A2X_PACK_SOUND_PH
