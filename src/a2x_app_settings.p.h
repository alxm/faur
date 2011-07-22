/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_APP_SETTINGS_PH
#define A2X_APP_SETTINGS_PH

#include "a2x_app_includes.h"

extern void a2x(void);

extern void a2x_set(const char* const key, const char* const val);
extern void a2x_flip(const char* const key);

extern char* a2x_str(const char* const key);
extern bool a2x_bool(const char* const key);
extern int a2x_int(const char* const key);

#endif // A2X_APP_SETTINGS_PH
