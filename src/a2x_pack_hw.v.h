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

#include "a2x_pack_hw.p.h"

#include "a2x_pack_file.v.h"
#include "a2x_pack_settings.v.h"

extern void a_hw__init_postSDL(void);
extern void a_hw__uninit(void);

#if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    extern uint32_t a_hw__getMs(void);
    extern void a_hw__setWizPortraitMode(void);
#endif
