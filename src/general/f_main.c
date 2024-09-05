/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_main.v.h"
#include <faur.v.h>

static int g_argsNum;
static const char** g_args;

#if !F_CONFIG_TRAIT_CUSTOM_MAIN
int main(int Argc, char* Argv[])
{
    g_argsNum = Argc;
    g_args = (const char**)Argv;

    f__main();

    while(f_state__runStep()) {
        continue;
    }

    f_init__uninit();

    return 0;
}
#endif

void f__main(void)
{
    f_init__init();
    f_state_push(f_main);
}

int f_main_argsNumGet(void)
{
    return g_argsNum;
}

const char* f_main_argsGet(int ArgNum)
{
    F__CHECK(ArgNum < g_argsNum);

    return g_args[ArgNum];
}
