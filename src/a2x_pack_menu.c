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

#include "a2x_pack_menu.h"

Menu* a_menu_set(Input* const next, Input* const back, Input* const select, Input* const cancel, void (*freeItem)(void* v))
{
    Menu* const m = malloc(sizeof(Menu));

    m->items = a_list_set();
    m->freeItem = freeItem;

    m->selectedNode = NULL;
    m->selectedIndex = 0;
    
    m->state = A_MENU_NOT_DONE;
    m->pause = A_MENU_PAUSE;
    m->used = 0;

    m->input = NULL;
    m->v = NULL;
    m->title = NULL;

    m->sprite = NULL;

    m->soundAccept = NULL;
    m->soundCancel = NULL;
    m->soundBrowse = NULL;

    m->next = next;
    m->back = back;
    m->select = select;
    m->cancel = cancel;

    return m;
}

void a_menu_free(Menu* const m)
{
    if(m->freeItem) {
        while(a_list_iterate(m->items)) {
            m->freeItem(a_list_current(m->items));
        }
    }

    a_list_free(m->items);
    free(m->title);
    free(m);
}

void a_menu_addInput(Menu* const m, void (*input)(struct Menu* const m, void* const v))
{
    m->input = input;
}

void a_menu_addV(Menu* const m, void* const v)
{
    m->v = v;
}

void a_menu_addTitle(Menu* const m, const char* const t)
{
    m->title = a_str_dup(t);
}

void a_menu_addSprite(Menu* const m, Sprite* const s)
{
    m->sprite = s;
}

void a_menu_addSounds(Menu* const m, Sound* const accept, Sound* const cancel, Sound* const browse)
{
    m->soundAccept = accept;
    m->soundCancel = cancel;
    m->soundBrowse = browse;
}

void a_menu_addItem(Menu* const m, void* const v)
{
    Node* const n = a_list_addLast(m->items, v);

    if(m->selectedNode == NULL) {
        m->selectedNode = n;
    }
}

void a_menu_input(Menu* const m)
{
    m->used = 0;

    if(!m->pause) {
        if(a_input_get(m->back)) {
            m->selectedIndex--;
            m->selectedNode = a_list_prev(m->selectedNode);
            m->used = 1;
        } else if(a_input_get(m->next)) {
            m->selectedIndex++;
            m->selectedNode = a_list_next(m->selectedNode);
            m->used = 1;
        }
    } else {
        if(!a_input_get(m->back) && !a_input_get(m->next) && !a_input_get(m->select) && !a_input_get(m->cancel)) {
            m->pause = 0;
        } else {
            m->pause--;
        }
    }

    if(m->used) {
        m->pause = A_MENU_PAUSE;

        if(m->soundBrowse) {
            a_sfx_play(m->soundBrowse);
        }

        if(m->selectedIndex < 0) {
            m->selectedNode = m->items->last->prev;
            m->selectedIndex = a_menu_items(m) - 1;
        } else if(m->selectedIndex == a_menu_items(m)) {
            m->selectedNode = m->items->first->next;
            m->selectedIndex = 0;
        }
    } else {
        if(a_input_get(m->select)) {
            m->state = A_MENU_ACCEPT;

            if(m->soundAccept) {
                a_sfx_play(m->soundAccept);
            }
        } else if(a_input_get(m->cancel)) {
            m->state = A_MENU_CANCEL;

            if(m->soundCancel) {
                a_sfx_play(m->soundCancel);
            }
        }
    }

    if(m->input) {
        m->input(m, m->v);
    }

    if(m->state != A_MENU_NOT_DONE) {
        a_input_unpress(m->next);
        a_input_unpress(m->back);
        a_input_unpress(m->select);
        a_input_unpress(m->cancel);
    }
}
