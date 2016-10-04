/*
    Copyright 2010, 2016 Alex Margarit

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

typedef struct AInputButton AInputButton;
typedef struct AInputAnalog AInputAnalog;
typedef struct AInputTouch AInputTouch;

typedef void (*AInputCallback)(void);

#include "a2x_pack_input.p.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_strhash.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_strbuilder.v.h"
#include "a2x_pack_strtok.v.h"

extern void a_input__init(void);
extern void a_input__uninit(void);

extern void a_input__free(AInput* Input);

extern void a_input__newController(void);

extern AInputButton* a_input__newButton(const char* Name);
extern AInputAnalog* a_input__newAnalog(const char* Name);
extern AInputTouch* a_input__newTouch(const char* Name);

extern void a_input__addCallback(AInputCallback Callback);
extern void a_input__get(void);

extern void a_input__button_setState(AInputButton* Button, bool Pressed);

extern void a_input__analog_setXAxis(AInputAnalog* Analog, int Value);
extern void a_input__analog_setYAxis(AInputAnalog* Analog, int Value);

extern void a_input__touch_addMotion(AInputTouch* Touch, int X, int Y);
extern void a_input__touch_setCoords(AInputTouch* Touch, int X, int Y, bool Tapped);
