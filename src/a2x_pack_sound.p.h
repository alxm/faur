/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

typedef struct AMusic AMusic;
typedef struct ASfx ASfx;

typedef enum {
    A_SFX_NORMAL = 0,
    A_SFX_LOOP = 1,
    A_SFX_RESTART = 2,
    A_SFX_YIELD = 4,
} ASfxFlags;

extern AMusic* a_music_new(const char* Path);
extern ASfx* a_sfx_dup(const ASfx* Sfx);
extern void a_music_free(AMusic* Music);

extern void a_music_play(AMusic* Music);
extern void a_music_stop(void);

extern ASfx* a_sfx_new(const char* Path);
extern void a_sfx_free(ASfx* Sfx);

extern void a_sfx_play(ASfx* Sfx, ASfxFlags Flags);
extern void a_sfx_stop(ASfx* Sfx);
extern bool a_sfx_isPlaying(ASfx* Sfx);
