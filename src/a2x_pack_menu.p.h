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

#define A2X_PACK_MENU_PH

#pragma once

typedef struct Menu Menu;

#include "a2x_pack_input.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_sound.p.h"
#include "a2x_pack_sprite.p.h"

extern Menu* a_menu_new(Input* next, Input* back, Input* select, Input* cancel, void (*freeItem)(void* v));
extern void a_menu_free(Menu* m);

extern void a_menu_addInput(Menu* m, void (*input)(Menu* m, void* v));
extern void a_menu_addV(Menu* m, void* v);
extern void a_menu_addTitle(Menu* m, const char* t);
extern void a_menu_addSprite(Menu* m, Sprite* s);
extern void a_menu_addSounds(Menu* m, Sound* accept, Sound* cancel, Sound* browse);
extern void a_menu_addItem(Menu* m, void* v);

extern void a_menu_input(Menu* m);
extern List* a_menu__items(const Menu* m);

#define A_MENU_ITERATE(menu, type, var) \
    A_LIST_ITERATE(a_menu__items(menu), type, var)

extern bool a_menu_isSelected(const Menu* m, const void* item);
extern void a_menu_keepRunning(Menu* m);
extern bool a_menu_running(const Menu* m);
extern bool a_menu_finished(const Menu* m);
extern bool a_menu_accept(const Menu* m);
extern bool a_menu_cancel(const Menu* m);
extern int a_menu_choice(const Menu* m);
