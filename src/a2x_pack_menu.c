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

#include "a2x_pack_menu.v.h"

typedef enum AMenuState {
    A_MENU_RUNNING, A_MENU_ACCEPT, A_MENU_CANCEL, A_MENU_SPENT
} AMenuState;

struct AMenu {
    AList* items;
    void (*freeItem)(void* v);
    int selectedIndex;
    void* selectedItem;
    AMenuState state;
    int pause;
    bool used;
    void (*input)(struct AMenu* const m, void* const v);
    void* v;
    char* title;
    ASprite* sprite;
    ASound* soundAccept;
    ASound* soundCancel;
    ASound* soundBrowse;
    AInput* next;
    AInput* back;
    AInput* select;
    AInput* cancel;
};

#define A_MENU_PAUSE (a_settings_getInt("fps.rate") / 6)

AMenu* a_menu_new(AInput* next, AInput* back, AInput* select, AInput* cancel, void (*freeItem)(void* v))
{
    AMenu* const m = a_mem_malloc(sizeof(AMenu));

    m->items = a_list_new();
    m->freeItem = freeItem;

    m->selectedIndex = 0;
    m->selectedItem = NULL;

    m->state = A_MENU_RUNNING;
    m->pause = A_MENU_PAUSE;
    m->used = false;

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

void a_menu_free(AMenu* m)
{
    if(m->freeItem) {
        A_LIST_ITERATE(m->items, void, v) {
            m->freeItem(v);
        }
    }

    a_list_free(m->items);
    free(m->title);

    free(m);
}

void a_menu_addInput(AMenu* m, void (*input)(AMenu* m, void* v))
{
    m->input = input;
}

void a_menu_addV(AMenu* m, void* v)
{
    m->v = v;
}

void a_menu_addTitle(AMenu* m, const char* t)
{
    m->title = a_str_dup(t);
}

void a_menu_addSprite(AMenu* m, ASprite* s)
{
    m->sprite = s;
}

void a_menu_addSounds(AMenu* m, ASound* accept, ASound* cancel, ASound* browse)
{
    m->soundAccept = accept;
    m->soundCancel = cancel;
    m->soundBrowse = browse;
}

void a_menu_addItem(AMenu* m, void* v)
{
    a_list_addLast(m->items, v);

    if(m->selectedItem == NULL) {
        m->selectedItem = v;
    }
}

void a_menu_input(AMenu* m)
{
    if(!a_menu_running(m)) {
        m->state = A_MENU_SPENT;
        return;
    }

    m->used = false;

    if(!m->pause) {
        if(a_button_get(m->back)) {
            m->selectedIndex--;
            m->selectedItem = a_list_get(m->items, m->selectedIndex);
            m->used = true;
        } else if(a_button_get(m->next)) {
            m->selectedIndex++;
            m->selectedItem = a_list_get(m->items, m->selectedIndex);
            m->used = true;
        }
    } else {
        if(!a_button_get(m->back)
            && !a_button_get(m->next)
            && !a_button_get(m->select)
            && !(m->cancel && a_button_get(m->cancel))) {
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
            m->selectedIndex = a_list__size(m->items) - 1;
            m->selectedItem = a_list_last(m->items);
        } else if(m->selectedIndex == a_list__size(m->items)) {
            m->selectedIndex = 0;
            m->selectedItem = a_list_first(m->items);
        }
    } else {
        if(a_button_get(m->select)) {
            m->state = A_MENU_ACCEPT;

            if(m->soundAccept) {
                a_sfx_play(m->soundAccept);
            }
        } else if(m->cancel && a_button_get(m->cancel)) {
            m->state = A_MENU_CANCEL;

            if(m->soundCancel) {
                a_sfx_play(m->soundCancel);
            }
        }
    }

    if(m->input) {
        m->input(m, m->v);
    }

    if(m->state != A_MENU_RUNNING) {
        a_button_unpress(m->next);
        a_button_unpress(m->back);
        a_button_unpress(m->select);

        if(m->cancel) {
            a_button_unpress(m->cancel);
        }
    }
}

AList* a_menu__items(const AMenu* m)
{
    return m->items;
}

bool a_menu_isSelected(const AMenu* m, const void* item)
{
    return item == m->selectedItem;
}

void a_menu_keepRunning(AMenu* m)
{
    m->state = A_MENU_RUNNING;
}

bool a_menu_running(const AMenu* m)
{
    return m->state == A_MENU_RUNNING;
}

bool a_menu_finished(const AMenu* m)
{
    return m->state == A_MENU_ACCEPT || m->state == A_MENU_CANCEL;
}

bool a_menu_accept(const AMenu* m)
{
    return m->state == A_MENU_ACCEPT;
}

bool a_menu_cancel(const AMenu* m)
{
    return m->state == A_MENU_CANCEL;
}

int a_menu_choice(const AMenu* m)
{
    return m->selectedIndex;
}
