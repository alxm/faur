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

#pragma once

#include "a2x_pack_sdl.p.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_pixel.v.h"

extern void a_sdl__init(void);
extern void a_sdl__uninit(void);

extern void a_sdl__screen_set(void);
extern void a_sdl__screen_show(void);

extern int a_sdl__sound_volumeMax(void);

extern void* a_sdl__music_load(const char* Path);
extern void a_sdl__music_free(void* Music);
extern void a_sdl__music_setVolume(void);
extern void a_sdl__music_play(void* Music);
extern void a_sdl__music_stop(void);
extern void a_sdl__music_toggle(void);

extern void* a_sdl__sfx_loadFromFile(const char* Path);
extern void* a_sdl__sfx_loadFromData(const uint8_t* Data, int Size);
extern void a_sdl__sfx_free(void* Sfx);
extern void a_sdl__sfx_setVolume(void* Sfx, int Volume);
extern void a_sdl__sfx_play(void* Sfx);

extern uint32_t a_sdl__getTicks(void);
extern void a_sdl__delay(uint32_t Milis);

extern void a_sdl__input_matchButton(const char* Name, APhysicalInput* Button);
extern void a_sdl__input_matchAnalog(const char* Name, APhysicalInput* Analog);
extern void a_sdl__input_matchTouch(const char* Name, APhysicalInput* Touch);
extern void a_sdl__input_get(void);
