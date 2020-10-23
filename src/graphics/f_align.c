/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#include "f_align.v.h"
#include <faur.v.h>

FAlign f__align;

static FList* g_stack;

static void f_align__init(void)
{
    g_stack = f_list_new();

    f_align_reset();
}

static void f_align__uninit(void)
{
    f_list_freeEx(g_stack, f_pool_release);
}

const FPack f_pack__align = {
    "Align",
    f_align__init,
    f_align__uninit,
};

void f_align_push(void)
{
    f_list_push(g_stack, f_pool__dup(F_POOL__STACK_ALIGN, &f__align));

    f_align_reset();
}

void f_align_pop(void)
{
    FAlign* align = f_list_pop(g_stack);

    #if F_CONFIG_DEBUG
        if(align == NULL) {
            F__FATAL("f_align_pop: Stack is empty");
        }
    #endif

    f__align = *align;
    f_pool_release(align);
}

void f_align_reset(void)
{
    f_align_set(F_ALIGN_X_LEFT, F_ALIGN_Y_TOP);
}

void f_align_set(FAlignX X, FAlignY Y)
{
    if(X != F_ALIGN_X_INVALID) {
        f__align.x = X;
    }

    if(Y != F_ALIGN_Y_INVALID) {
        f__align.y = Y;
    }
}
