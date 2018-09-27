/*
    Copyright 2010, 2017, 2018 Alex Margarit

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

typedef struct AButton AButton;

extern AButton* a_button_new(const char* Ids);
extern AButton* a_button_dup(const AButton* Button);
extern void a_button_free(AButton* Button);

extern bool a_button_isWorking(const AButton* Button);
extern const char* a_button_nameGet(const AButton* Button);

extern bool a_button_pressGet(const AButton* Button);
extern bool a_button_pressGetOnce(const AButton* Button);
extern void a_button_pressSetRepeat(AButton* Button, unsigned RepeatMs);
extern void a_button_pressClear(const AButton* Button);
