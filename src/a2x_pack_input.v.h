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

typedef struct AInputUserHeader AInputUserHeader;
typedef struct AInputSourceHeader AInputSourceHeader;

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

#include "a2x_pack_input_button.v.h"
#include "a2x_pack_input_analog.v.h"
#include "a2x_pack_input_controller.v.h"
#include "a2x_pack_input_touch.v.h"

struct AInputUserHeader {
    char* name;
    AList* sourceInputs; // List of AInputButtonSource/Analog/Touch
};

struct AInputSourceHeader {
    char* name;
    unsigned lastEventFrame;
};

extern void a_input__init(void);
extern void a_input__uninit(void);

extern void a_input__addCallback(AInputCallback Callback);

extern void a_input__initUserHeader(AInputUserHeader* Header);
extern void a_input__freeUserHeader(AInputUserHeader* Header);
extern void a_input__initSourceHeader(AInputSourceHeader* Header, const char* Name);
extern void a_input__freeSourceHeader(AInputSourceHeader* Header);

extern void a_input__findSourceInput(const AStrHash* GlobalCollection, const AStrHash* ControllerCollection, const char* Id, AInputUserHeader* UserInput);

extern bool a_input__hasFreshEvent(const AInputSourceHeader* Header);
extern void a_input__setFreshEvent(AInputSourceHeader* Header);

extern void a_input__get(void);
