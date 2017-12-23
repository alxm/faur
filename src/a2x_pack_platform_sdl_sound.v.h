/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_platform_sdl_sound.p.h"

typedef struct ASdlSfx ASdlSfx;

#include "a2x_pack_out.v.h"

extern void a_sdl_sound__init(void);
extern void a_sdl_sound__uninit(void);

extern int a_sdl_sound__getMaxVolome(void);

extern void* a_sdl_sound__musicLoad(const char* Path);
extern void a_sdl_sound__musicFree(void* Music);
extern void a_sdl_sound__musicSetVolume(int Volume);
extern void a_sdl_sound__musicPlay(void* Music);
extern void a_sdl_sound__musicStop(void);
extern void a_sdl_sound__musicToggle(void);

extern ASdlSfx* a_sdl_sound__sfxLoadFromFile(const char* Path);
extern ASdlSfx* a_sdl_sound__sfxLoadFromData(const uint8_t* Data, int Size);
extern void a_sdl_sound__sfxFree(ASdlSfx* Sfx);
extern void a_sdl_sound__sfxSetVolume(ASdlSfx* Sfx, int Volume);
extern void a_sdl_sound__sfxPlay(ASdlSfx* Sfx);
extern void a_sdl_sound__sfxStop(ASdlSfx* Sfx);
extern bool a_sdl_sound__sfxIsPlaying(ASdlSfx* Sfx);
