/*
    Copyright 2020 Alex Margarit <alex@alxm.org>
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

#define FAUR_IMPLEMENT_SYM

#include "f_sym.v.h"
#include <faur.v.h>

static FHash* g_map;

static void f_sym__init(void)
{
    g_map = f_hash_newStr(256, false);
}

static void f_sym__uninit(void)
{
    f_hash_free(g_map);
}

const FPack f_pack__sym = {
    "Sym",
    f_sym__init,
    f_sym__uninit,
};

void f_sym_set(const char* Name, uintptr_t Value)
{
    F__CHECK(Name != NULL);

    f_hash_add(g_map, Name, (void*)Value);
}

bool f_sym_test(const char* Name)
{
    F__CHECK(Name != NULL);

    return f_hash_contains(g_map, Name);
}

int f_sym_int(const char* Name)
{
    F__CHECK(Name != NULL);

    if(*Name == '\0') {
        return 0;
    }

    if(!f_hash_contains(g_map, Name)) {
        F__FATAL("f_sym_int(%s): Not found", Name);
    }

    return (int)(intptr_t)f_hash_get(g_map, Name);
}

unsigned f_sym_intu(const char* Name)
{
    F__CHECK(Name != NULL);

    if(*Name == '\0') {
        return 0;
    }

    if(!f_hash_contains(g_map, Name)) {
        F__FATAL("f_sym_intu(%s): Not found", Name);
    }

    return (unsigned)(uintptr_t)f_hash_get(g_map, Name);
}

uintptr_t f_sym_address(const char* Name)
{
    F__CHECK(Name != NULL);

    if(*Name == '\0') {
        return 0;
    }

    if(!f_hash_contains(g_map, Name)) {
        F__FATAL("f_sym_address(%s): Not found", Name);
    }

    return (uintptr_t)f_hash_get(g_map, Name);
}
