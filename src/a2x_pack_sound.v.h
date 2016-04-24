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

#include "a2x_pack_sound.p.h"

#include "a2x_app_settings.v.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_mem.v.h"

#define A_VOLUME_STEP 1
#define A_MILIS_VOLUME (1000 / 2)

extern int a__volume;
extern int a__volumeMax;
extern int a__volumeAdjust;

extern void a_sound__init(void);
extern void a_sound__uninit(void);

extern void a_sfx__free(Sound* s);
extern void a_music__free(Music* m);
