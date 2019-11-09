/*
    Copyright 2010, 2017-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_INPUTS_ANALOG_P_H
#define F_INC_INPUTS_ANALOG_P_H

#include "../general/f_system_includes.h"

typedef struct FAnalog FAnalog;

typedef enum {
    F_AXIS_INVALID = -1,
    F_AXIS_LEFTX,
    F_AXIS_LEFTY,
    F_AXIS_RIGHTX,
    F_AXIS_RIGHTY,
    F_AXIS_LEFTTRIGGER,
    F_AXIS_RIGHTTRIGGER,
    F_AXIS_NUM
} FAnalogId;

#include "../input/f_controller.v.h"
#include "../math/f_fix.p.h"

extern FAnalog* f_analog_new(void);
extern void f_analog_free(FAnalog* Analog);

extern void f_analog_bind(FAnalog* Analog, const FController* Controller, FAnalogId Id);

extern bool f_analog_isWorking(const FAnalog* Analog);
extern const char* f_analog_nameGet(const FAnalog* Analog);

extern FFix f_analog_valueGet(const FAnalog* Analog);

#endif // F_INC_INPUTS_ANALOG_P_H
