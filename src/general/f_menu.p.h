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

#ifndef F_INC_GENERAL_MENU_P_H
#define F_INC_GENERAL_MENU_P_H

#include "../general/f_system_includes.h"

typedef struct FMenu FMenu;

typedef enum {
    F_MENU_STATE_INVALID = -1,
    F_MENU_STATE_RUNNING,
    F_MENU_STATE_SELECTED,
    F_MENU_STATE_CANCELED
} FMenuState;

#include "../data/f_list.p.h"
#include "../input/f_button.p.h"
#include "../sound/f_sample.p.h"

extern FMenu* f_menu_new(FButton* Next, FButton* Back, FButton* Select, FButton* Cancel);
extern void f_menu_free(FMenu* Menu);
extern void f_menu_freeEx(FMenu* Menu, FFree* ItemFree);

extern void f_menu_soundSet(FMenu* Menu, FSample* Accept, FSample* Cancel, FSample* Browse);
extern void f_menu_itemAdd(FMenu* Menu, void* Item);

extern void f_menu_tick(FMenu* Menu);
extern FMenuState f_menu_stateGet(const FMenu* Menu);

extern const FList* f_menu_itemsGet(const FMenu* Menu);
extern bool f_menu_itemIsSelected(const FMenu* Menu, const void* Item);
extern unsigned f_menu_selectedIndexGet(const FMenu* Menu);
extern void* f_menu_itemGetSelected(const FMenu* Menu);

extern void f_menu_keepRunning(FMenu* Menu);
extern void f_menu_reset(FMenu* Menu);

#endif // F_INC_GENERAL_MENU_P_H
