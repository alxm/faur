/*
    Copyright 2010, 2016 Alex Margarit

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

typedef struct AMenu AMenu;

typedef enum {
    A_MENU_STATE_INVALID = -1,
    A_MENU_STATE_RUNNING,
    A_MENU_STATE_SELECTED,
    A_MENU_STATE_CANCELED
} AMenuState;

#include "a2x_pack_input_button.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_sound.p.h"

extern AMenu* a_menu_new(AInputButton* Next, AInputButton* Back, AInputButton* Select, AInputButton* Cancel);
extern void a_menu_free(AMenu* Menu);
extern void a_menu_freeEx(AMenu* Menu, AFree* ItemFree);

extern void a_menu_addSounds(AMenu* Menu, ASound* Accept, ASound* Cancel, ASound* Browse);
extern void a_menu_addItem(AMenu* Menu, void* Item);

extern void a_menu_handleInput(AMenu* Menu);
extern AMenuState a_menu_getState(const AMenu* Menu);

extern AList* a_menu_getItems(const AMenu* Menu);
extern bool a_menu_isItemSelected(const AMenu* Menu, const void* Item);
extern unsigned a_menu_getSelectedIndex(const AMenu* Menu);
extern void* a_menu_getSelectedItem(const AMenu* Menu);

extern void a_menu_keepRunning(AMenu* Menu);
extern void a_menu_reset(AMenu* Menu);
