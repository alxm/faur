/*
    Copyright 2010, 2016-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_input.v.h"

#include "a2x_pack_input_controller.v.h"
#include "a2x_pack_platform.v.h"

const char* a__inputNameDefault = "Unknown";

void a_input__init(void)
{
    a_input_button__init();
    a_input_controller__init();
}

void a_input__uninit(void)
{
    a_input_button__uninit();
}

void a_input__userHeaderInit(AInputUserHeader* Header)
{
    Header->name = a__inputNameDefault;
    Header->platformInputs = a_list_new();
}

void a_input__userHeaderFree(AInputUserHeader* Header)
{
    a_list_free(Header->platformInputs);
}

void a_input__tick(void)
{
    a_platform__inputPoll();
    a_input_button__tick();
}
