/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#include "a2x_pack_settings.p.h"

typedef enum {
    A__SETTING_TYPE_INVALID = -1,
    A__SETTING_TYPE_INT,
    A__SETTING_TYPE_INTU,
    A__SETTING_TYPE_BOOL,
    A__SETTING_TYPE_STR,
    A__SETTING_TYPE_PIXEL,
    A__SETTING_TYPE_NUM
} ASettingType;

extern void a_settings__init(void);
extern void a_settings__uninit(void);

extern ASettingId a_settings__stringToId(const char* Key);
extern ASettingType a_settings__typeGet(ASettingId Setting);
