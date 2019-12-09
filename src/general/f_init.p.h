/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_INIT_P_H
#define F_INC_GENERAL_INIT_P_H

#include "../general/f_system_includes.h"

extern void f_init(void);

extern void f_init_app(const char* Name, const char* Author, uint8_t VerMajor, uint8_t VerMinor, uint8_t VerMicro);
extern void f_init_fps(unsigned TickRate, unsigned DrawRate);

#endif // F_INC_GENERAL_INIT_P_H
