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

extern bool a_sdl__screen_set(void);
extern Pixel* a_sdl__screen_pixels(void);
extern void a_sdl__screen_lock(void);
extern void a_sdl__screen_unlock(void);
extern void a_sdl__screen_flip(void);

extern void* a_sdl__sfx_loadFromFile(const char* path);
extern void* a_sdl__sfx_loadFromData(const uint16_t* data, int size);
extern void a_sdl__sfx_setVolume(void* s, uint8_t volume);

extern uint32_t a_sdl__getTicks(void);
extern void a_sdl__delay(uint32_t ms);

extern void a_sdl__input_init(void);
extern void a_sdl__input_free(void);
extern void a_sdl__input_matchButton(const char* name, InputInstance* button);
extern void a_sdl__input_matchAnalog(const char* name, InputInstance* analog);
extern void a_sdl__input_matchTouch(const char* name, InputInstance* touch);
extern void a_sdl__input_get(void);
