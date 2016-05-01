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

typedef struct InputInstance InputInstance;

#include "a2x_pack_input.p.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_strhash.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_strtok.v.h"

typedef struct InputCollection {
    List* list; // inputs registered during init
    StrHash* names; // hash table of above inputs' names
} InputCollection;

extern void a_input__init(void);
extern void a_input__uninit(void);

extern void a_input__free(Input* i);

extern void a_input__get(void);

extern void a_input__button_setState(InputInstance* b, bool pressed);

extern void a_input__analog_setXAxis(InputInstance* a, int value);
extern void a_input__analog_setYAxis(InputInstance* a, int value);

extern void a_input__touch_addMotion(InputInstance* t, int x, int y);
extern void a_input__touch_setCoords(InputInstance* t, int x, int y, bool tapped);

extern InputCollection* a_input__collection_new(void);
extern void a_input__collection_free(InputCollection* c);
extern void a_input__collection_add(InputCollection* c, void* instance, const char* name);
