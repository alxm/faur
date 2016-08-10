/*
    Copyright 2010 Alex Margarit

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
typedef void (*AMenuFreeItemHandler)(void* Item);
typedef void (*AMenuInputHandler)(AMenu* Menu, void* Context);

#include "a2x_pack_input.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_sound.p.h"
#include "a2x_pack_sprite.p.h"

extern AMenu* a_menu_new(AInput* Next, AInput* Back, AInput* Select, AInput* Cancel, AMenuFreeItemHandler FreeItemHandler);
extern void a_menu_free(AMenu* Menu);

extern void a_menu_addInput(AMenu* Menu, AMenuInputHandler InputHandler);
extern void a_menu_addContext(AMenu* Menu, void* Context);
extern void a_menu_addTitle(AMenu* Menu, const char* Title);
extern void a_menu_addSprite(AMenu* Menu, ASprite* Sprite);
extern void a_menu_addSounds(AMenu* Menu, ASound* Accept, ASound* Cancel, ASound* Browse);
extern void a_menu_addItem(AMenu* Menu, void* Item);

extern void a_menu_input(AMenu* Menu);
extern AList* a_menu__items(const AMenu* Menu);

#define A_MENU_ITERATE(Menu, Item)            \
    A_LIST_ITERATE(a_menu__items(Menu), Item)

extern bool a_menu_isSelected(const AMenu* Menu, const void* Item);
extern void a_menu_keepRunning(AMenu* Menu);
extern bool a_menu_running(const AMenu* Menu);
extern bool a_menu_finished(const AMenu* Menu);
extern bool a_menu_accept(const AMenu* Menu);
extern bool a_menu_cancel(const AMenu* Menu);
extern int a_menu_choice(const AMenu* Menu);
