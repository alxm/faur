/*
    Copyright 2017 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_SYSTEM_WIZ_V_H
#define F_INC_PLATFORM_SYSTEM_WIZ_V_H

#include "f_wiz.p.h"

extern void f_platform_wiz__init(void);
extern void f_platform_wiz__uninit(void);

extern uint32_t f_platform_api_wiz__timeMsGet(void);
extern void f_platform_api_wiz___timeMsWait(uint32_t Ms);

extern void f_platform_wiz__portraitModeSet(void);

#endif // F_INC_PLATFORM_SYSTEM_WIZ_V_H
