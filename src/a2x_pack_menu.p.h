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

#ifndef A2X_PACK_MENU_PH
#define A2X_PACK_MENU_PH

#include "a2x_app_includes.h"

typedef struct Menu Menu;

#include "a2x_pack_input.p.h"
#include "a2x_pack_sound.p.h"
#include "a2x_pack_sprite.p.h"

extern Menu* a_menu_set(Input* const next, Input* const back, Input* const select, Input* const cancel, void (*freeItem)(void* v));
extern void a_menu_free(Menu* const m);

extern void a_menu_addInput(Menu* const m, void (*input)(struct Menu* const m, void* const v));
extern void a_menu_addV(Menu* const m, void* const v);
extern void a_menu_addTitle(Menu* const m, const char* const t);
extern void a_menu_addSprite(Menu* const m, Sprite* const s);
extern void a_menu_addSounds(Menu* const m, Sound* const accept, Sound* const cancel, Sound* const browse);
extern void a_menu_addItem(Menu* const m, void* const v);

extern void a_menu_input(Menu* const m);

extern int a_menu_items(const Menu* const m);
extern bool a_menu_iterate(const Menu* const m);
extern void a_menu_iterateReset(const Menu* const m);
extern void* a_menu_currentItem(const Menu* const m);
extern bool a_menu_isSelected(const Menu* const m);
extern void a_menu_keepRunning(Menu* const m);
extern bool a_menu_running(const Menu* const m);
extern bool a_menu_finished(const Menu* const m);
extern bool a_menu_accept(const Menu* const m);
extern bool a_menu_cancel(const Menu* const m);
extern int a_menu_choice(const Menu* const m);

#endif // A2X_PACK_MENU_PH
