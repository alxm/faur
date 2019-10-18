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

#include "f_menu.v.h"
#include <faur.v.h>

struct AMenu {
    AMenuState state;
    AList* items;
    void* selectedItem;
    unsigned selectedIndex;
    ASample* soundAccept;
    ASample* soundCancel;
    ASample* soundBrowse;
    AButton* next;
    AButton* back;
    AButton* select;
    AButton* cancel;
};

AMenu* f_menu_new(AButton* Next, AButton* Back, AButton* Select, AButton* Cancel)
{
    AMenu* m = f_mem_malloc(sizeof(AMenu));

    m->state = A_MENU_STATE_RUNNING;
    m->items = f_list_new();
    m->selectedItem = NULL;
    m->selectedIndex = 0;
    m->soundAccept = NULL;
    m->soundCancel = NULL;
    m->soundBrowse = NULL;
    m->next = f_button_dup(Next);
    m->back = f_button_dup(Back);
    m->select = Select;
    m->cancel = Cancel;

    f_button_pressSetRepeat(m->next, 200);
    f_button_pressSetRepeat(m->back, 200);

    return m;
}

void f_menu_free(AMenu* Menu)
{
    f_menu_freeEx(Menu, NULL);
}

void f_menu_freeEx(AMenu* Menu, AFree* ItemFree)
{
    if(Menu == NULL) {
        return;
    }

    f_list_freeEx(Menu->items, ItemFree);
    f_button_free(Menu->next);
    f_button_free(Menu->back);

    f_mem_free(Menu);
}

void f_menu_soundSet(AMenu* Menu, ASample* Accept, ASample* Cancel, ASample* Browse)
{
    Menu->soundAccept = Accept;
    Menu->soundCancel = Cancel;
    Menu->soundBrowse = Browse;
}

void f_menu_itemAdd(AMenu* Menu, void* Item)
{
    f_list_addLast(Menu->items, Item);

    if(Menu->selectedItem == NULL) {
        Menu->selectedItem = Item;
    }
}

void f_menu_tick(AMenu* Menu)
{
    if(f_list_isEmpty(Menu->items) || Menu->state != A_MENU_STATE_RUNNING) {
        return;
    }

    bool browsed = false;

    if(f_button_pressGet(Menu->back)) {
        browsed = true;

        if(Menu->selectedIndex-- == 0) {
            Menu->selectedIndex = f_list_sizeGet(Menu->items) - 1;
        }
    } else if(f_button_pressGet(Menu->next)) {
        browsed = true;

        if(++Menu->selectedIndex == f_list_sizeGet(Menu->items)) {
            Menu->selectedIndex = 0;
        }
    }

    if(browsed) {
        Menu->selectedItem = f_list_getByIndex(
                                Menu->items, Menu->selectedIndex);

        if(Menu->soundBrowse) {
            f_channel_play(A_CHANNEL_ANY, Menu->soundBrowse, A_CHANNEL_NORMAL);
        }
    } else {
        if(f_button_pressGet(Menu->select)) {
            Menu->state = A_MENU_STATE_SELECTED;

            if(Menu->soundAccept) {
                f_channel_play(
                    A_CHANNEL_ANY, Menu->soundAccept, A_CHANNEL_NORMAL);
            }
        } else if(Menu->cancel && f_button_pressGet(Menu->cancel)) {
            Menu->state = A_MENU_STATE_CANCELED;

            if(Menu->soundCancel) {
                f_channel_play(
                    A_CHANNEL_ANY, Menu->soundCancel, A_CHANNEL_NORMAL);
            }
        }
    }

    if(Menu->state != A_MENU_STATE_RUNNING) {
        f_button_pressClear(Menu->next);
        f_button_pressClear(Menu->back);
        f_button_pressClear(Menu->select);

        if(Menu->cancel) {
            f_button_pressClear(Menu->cancel);
        }
    }
}

AMenuState f_menu_stateGet(const AMenu* Menu)
{
    return Menu->state;
}

const AList* f_menu_itemsGet(const AMenu* Menu)
{
    return Menu->items;
}

bool f_menu_itemIsSelected(const AMenu* Menu, const void* Item)
{
    return Item == Menu->selectedItem;
}

unsigned f_menu_selectedIndexGet(const AMenu* Menu)
{
    return Menu->selectedIndex;
}

void* f_menu_itemGetSelected(const AMenu* Menu)
{
    return Menu->selectedItem;
}

void f_menu_keepRunning(AMenu* Menu)
{
    Menu->state = A_MENU_STATE_RUNNING;
}

void f_menu_reset(AMenu* Menu)
{
    Menu->state = A_MENU_STATE_RUNNING;
    Menu->selectedItem = f_list_getFirst(Menu->items);
    Menu->selectedIndex = 0;
}
