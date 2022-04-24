/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_INPUTS_CONTROLLER_P_H
#define F_INC_INPUTS_CONTROLLER_P_H

#include "../general/f_system_includes.h"

typedef void FController;
typedef void FCallControllerBind(const FController* Controller);

extern FController* f_controller_new(FCallControllerBind* Callback);
extern void f_controller_free(FController* Controller);

#endif // F_INC_INPUTS_CONTROLLER_P_H
