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

#ifndef A2X_PACK_MENU_H
#define A2X_PACK_MENU_H

#include <stdlib.h>

#include "a2x_app_settings.h"

#include "a2x_pack_blit.h"
#include "a2x_pack_fps.h"
#include "a2x_pack_input.h"
#include "a2x_pack_sound.h"

#define A_MENU_PAUSE (a2xSet.fps / 6)

typedef enum MenuState {
    A_MENU_NOT_DONE, A_MENU_ACCEPT, A_MENU_CANCEL
} MenuState;

typedef struct Menu {
    List* items;
    void (*freeItem)(void* v);
    int currentIndex;
    MenuState state;
    int pause;
    int used;
    void (*input)(struct Menu* const m, void* const v);
    void* v;
    char* title;
    Sprite* sprite;
    Sound* soundAccept;
    Sound* soundCancel;
    Sound* soundBrowse;
    Input* next;
    Input* back;
    Input* select;
    Input* cancel;
} Menu;

extern Menu* a_menu_set(Input* const next, Input* const back, Input* const select, Input* const cancel, void (*freeItem)(void* v));
extern void a_menu_free(Menu* const m);

extern void a_menu_addInput(Menu* const m, void (*input)(struct Menu* const m, void* const v));
extern void a_menu_addV(Menu* const m, void* const v);
extern void a_menu_addTitle(Menu* const m, const char* const t);
extern void a_menu_addSprite(Menu* const m, Sprite* const s);
extern void a_menu_addSounds(Menu* const m, Sound* const accept, Sound* const cancel, Sound* const browse);

extern void a_menu_addItem(Menu* const m, void* const v);

extern void a_menu_input(Menu* const m);

#define a_menu_items(m) (a_list_items(m->items))

#define a_menu_iterate(m)      (a_list_iterate(m->items))
#define a_menu_iterateReset(m) (a_list_reset(m->items))
#define a_menu_currentItem(m)  (a_list_current(m->items))

#define a_menu_reset(m)   (m->state = MENU_NOT_DONE)
#define a_menu_notDone(m) (m->state == MENU_NOT_DONE)
#define a_menu_accept(m)  (m->state == MENU_ACCEPT)
#define a_menu_cancel(m)  (m->state == MENU_CANCEL)

#define a_menu_choiceNumber(m)  (m->currentIndex)

#endif // A2X_PACK_MENU_H
