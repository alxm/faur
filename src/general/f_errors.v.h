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

#ifndef F_INC_GENERAL_ERRORS_V_H
#define F_INC_GENERAL_ERRORS_V_H

#include "f_errors.p.h"

#define F_DEBUG_FATAL_DEFAULT 0
#define F_DEBUG_FATAL_SEGFAULT 1
#define F_DEBUG_FATAL_SPIN 2
#define F_DEBUG_FATAL_WAIT 3

#define F__CHECK(Condition) \
    if(F_CONFIG_DEBUG && !(Condition)) \
        F__FATAL( \
            "%s: Failed check `%s`", \
            __func__, \
            #Condition )

extern void F__FATAL(const char* Format, ...) F__ATTRIBUTE_NORETURN F__ATTRIBUTE_FORMAT(1);

#endif // F_INC_GENERAL_ERRORS_V_H
