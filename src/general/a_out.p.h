/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_GENERAL_OUT_P_H
#define A_INC_GENERAL_OUT_P_H

#include "general/a_system_includes.h"

#if A_CONFIG_OUTPUT_ENABLED
    extern void a_out_text(const char* Text);
    extern void a_out_info(const char* Format, ...);
    extern void a_out_warning(const char* Format, ...);
    extern void a_out_error(const char* Format, ...);
#else
    #define a_out_text(Text)
    #define a_out_info(...)
    #define a_out_warning(...)
    #define a_out_error(...)
#endif

#endif // A_INC_GENERAL_OUT_P_H
