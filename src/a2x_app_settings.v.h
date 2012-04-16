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

#ifndef A2X_APP_SETTINGS_VH
#define A2X_APP_SETTINGS_VH

#include "a2x_app_settings.p.h"

#include "a2x_app_main.v.h"

#include "a2x_pack_hash.v.h"
#include "a2x_pack_str.v.h"

extern void a_settings__defaults(void);
extern void a_settings__freeze(void);

extern void a2x__set(const char* key, const char* val);
extern bool a2x__flip(const char* key);

#endif // A2X_APP_SETTINGS_VH
