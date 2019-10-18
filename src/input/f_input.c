/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_input.v.h"
#include <faur.v.h>

static void f_input__init(void)
{
    f_input_button__init();
}

static void f_input__uninit(void)
{
    f_input_button__uninit();
}

const FPack f_pack__input = {
    "Input",
    {
        [0] = f_input__init,
    },
    {
        [0] = f_input__uninit,
    },
};

void f_input__tick(void)
{
    f_platform_api__inputPoll();
    f_input_button__tick();
}

bool f_input_readKey(FKeyId Key)
{
    const FPlatformButton* k = f_platform_api__inputKeyGet(Key);

    return k ? f_platform_api__inputButtonPressGet(k) : false;
}

bool f_input_readButton(FButtonId Button)
{
    const FPlatformButton* b = f_platform_api__inputButtonGet(NULL, Button);

    return b ? f_platform_api__inputButtonPressGet(b) : false;
}

FFix f_input_readAnalog(FAnalogId Axis)
{
    const FPlatformAnalog* a = f_platform_api__inputAnalogGet(NULL, Axis);

    return a ? f_platform_api__inputAnalogValueGet(a) : 0;
}
