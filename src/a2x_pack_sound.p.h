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

#include "a2x_system_includes.h"

typedef void AMusic;
typedef void ASound;

extern AMusic* a_music_load(const char* Path);

extern void a_music_play(AMusic* Music);
extern void a_music_stop(void);

extern ASound* a_sfx_fromFile(const char* Path);
#define a_sfx_fromData(Data) a_sfx__fromData(Data, sizeof(Data))
extern ASound* a_sfx__fromData(const uint8_t* Data, int Size);

extern void a_sfx_play(ASound* Sfx);
extern void a_sfx_volume(int Volume);
