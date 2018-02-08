/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_pixel.p.h"

#define A_SETUP void a_settings__application(void)
extern A_SETUP;

extern void a_settings_set(const char* Key, const char* Value);
extern bool a_settings_flip(const char* Key);

extern const char* a_settings_getString(const char* Key);
extern bool a_settings_getBool(const char* Key);
extern int a_settings_getInt(const char* Key);
extern unsigned a_settings_getUnsigned(const char* Key);
extern APixel a_settings_getPixel(const char* Key);
