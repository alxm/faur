/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_GENERAL_MENU_P_H
#define A_INC_GENERAL_MENU_P_H

#include "general/f_system_includes.h"

typedef struct AMenu AMenu;

typedef enum {
    A_MENU_STATE_INVALID = -1,
    A_MENU_STATE_RUNNING,
    A_MENU_STATE_SELECTED,
    A_MENU_STATE_CANCELED
} AMenuState;

#include "data/f_list.p.h"
#include "input/f_button.p.h"
#include "sound/f_sound.p.h"

extern AMenu* f_menu_new(AButton* Next, AButton* Back, AButton* Select, AButton* Cancel);
extern void f_menu_free(AMenu* Menu);
extern void f_menu_freeEx(AMenu* Menu, AFree* ItemFree);

extern void f_menu_soundSet(AMenu* Menu, ASample* Accept, ASample* Cancel, ASample* Browse);
extern void f_menu_itemAdd(AMenu* Menu, void* Item);

extern void f_menu_tick(AMenu* Menu);
extern AMenuState f_menu_stateGet(const AMenu* Menu);

extern const AList* f_menu_itemsGet(const AMenu* Menu);
extern bool f_menu_itemIsSelected(const AMenu* Menu, const void* Item);
extern unsigned f_menu_selectedIndexGet(const AMenu* Menu);
extern void* f_menu_itemGetSelected(const AMenu* Menu);

extern void f_menu_keepRunning(AMenu* Menu);
extern void f_menu_reset(AMenu* Menu);

#endif // A_INC_GENERAL_MENU_P_H
