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
    A_MENU_RUNNING,
    A_MENU_ACCEPT,
    A_MENU_CANCEL,
    A_MENU_SPENT
} AMenuState;

struct AMenu {
    AList* items;
    AMenuFreeItemHandler freeItem;
    int selectedIndex;
    void* selectedItem;
    AMenuState state;
    int pause;
    bool used;
    AMenuInputHandler inputHandler;
    void* context;
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

AMenu* a_menu_new(AInput* Next, AInput* Back, AInput* Select, AInput* Cancel, AMenuFreeItemHandler FreeItemHandler)
{
    AMenu* const m = a_mem_malloc(sizeof(AMenu));

    m->items = a_list_new();
    m->freeItem = FreeItemHandler;

    m->selectedIndex = 0;
    m->selectedItem = NULL;

    m->state = A_MENU_RUNNING;
    m->pause = A_MENU_PAUSE;
    m->used = false;

    m->inputHandler = NULL;
    m->context = NULL;
    m->title = NULL;

    m->sprite = NULL;

    m->soundAccept = NULL;
    m->soundCancel = NULL;
    m->soundBrowse = NULL;

    m->next = Next;
    m->back = Back;
    m->select = Select;
    m->cancel = Cancel;

    return m;
}

void a_menu_free(AMenu* Menu)
{
    if(Menu->freeItem) {
        A_LIST_ITERATE(Menu->items, void, v) {
            Menu->freeItem(v);
        }
    }

    a_list_free(Menu->items);
    free(Menu->title);

    free(Menu);
}

void a_menu_addInput(AMenu* Menu, AMenuInputHandler InputHandler)
{
    Menu->inputHandler = InputHandler;
}

void a_menu_addContext(AMenu* Menu, void* Context)
{
    Menu->context = Context;
}

void a_menu_addTitle(AMenu* Menu, const char* Title)
{
    Menu->title = a_str_dup(Title);
}

void a_menu_addSprite(AMenu* Menu, ASprite* Sprite)
{
    Menu->sprite = Sprite;
}

void a_menu_addSounds(AMenu* Menu, ASound* Accept, ASound* Cancel, ASound* Browse)
{
    Menu->soundAccept = Accept;
    Menu->soundCancel = Cancel;
    Menu->soundBrowse = Browse;
}

void a_menu_addItem(AMenu* Menu, void* Item)
{
    a_list_addLast(Menu->items, Item);

    if(Menu->selectedItem == NULL) {
        Menu->selectedItem = Item;
    }
}

void a_menu_input(AMenu* Menu)
{
    if(!a_menu_running(Menu)) {
        Menu->state = A_MENU_SPENT;
        return;
    }

    Menu->used = false;

    if(!Menu->pause) {
        if(a_button_get(Menu->back)) {
            Menu->selectedIndex--;
            Menu->selectedItem = a_list_get(Menu->items, Menu->selectedIndex);
            Menu->used = true;
        } else if(a_button_get(Menu->next)) {
            Menu->selectedIndex++;
            Menu->selectedItem = a_list_get(Menu->items, Menu->selectedIndex);
            Menu->used = true;
        }
    } else {
        if(!a_button_get(Menu->back)
            && !a_button_get(Menu->next)
            && !a_button_get(Menu->select)
            && !(Menu->cancel && a_button_get(Menu->cancel))) {
            Menu->pause = 0;
        } else {
            Menu->pause--;
        }
    }

    if(Menu->used) {
        Menu->pause = A_MENU_PAUSE;

        if(Menu->soundBrowse) {
            a_sfx_play(Menu->soundBrowse);
        }

        if(Menu->selectedIndex < 0) {
            Menu->selectedIndex = a_list__size(Menu->items) - 1;
            Menu->selectedItem = a_list_last(Menu->items);
        } else if(Menu->selectedIndex == a_list__size(Menu->items)) {
            Menu->selectedIndex = 0;
            Menu->selectedItem = a_list_first(Menu->items);
        }
    } else {
        if(a_button_get(Menu->select)) {
            Menu->state = A_MENU_ACCEPT;

            if(Menu->soundAccept) {
                a_sfx_play(Menu->soundAccept);
            }
        } else if(Menu->cancel && a_button_get(Menu->cancel)) {
            Menu->state = A_MENU_CANCEL;

            if(Menu->soundCancel) {
                a_sfx_play(Menu->soundCancel);
            }
        }
    }

    if(Menu->inputHandler) {
        Menu->inputHandler(Menu, Menu->context);
    }

    if(Menu->state != A_MENU_RUNNING) {
        a_button_unpress(Menu->next);
        a_button_unpress(Menu->back);
        a_button_unpress(Menu->select);

        if(Menu->cancel) {
            a_button_unpress(Menu->cancel);
        }
    }
}

AList* a_menu__items(const AMenu* Menu)
{
    return Menu->items;
}

bool a_menu_isSelected(const AMenu* Menu, const void* Item)
{
    return Item == Menu->selectedItem;
}

void a_menu_keepRunning(AMenu* Menu)
{
    Menu->state = A_MENU_RUNNING;
}

bool a_menu_running(const AMenu* Menu)
{
    return Menu->state == A_MENU_RUNNING;
}

bool a_menu_finished(const AMenu* Menu)
{
    return Menu->state == A_MENU_ACCEPT || Menu->state == A_MENU_CANCEL;
}

bool a_menu_accept(const AMenu* Menu)
{
    return Menu->state == A_MENU_ACCEPT;
}

bool a_menu_cancel(const AMenu* Menu)
{
    return Menu->state == A_MENU_CANCEL;
}

int a_menu_choice(const AMenu* Menu)
{
    return Menu->selectedIndex;
}
