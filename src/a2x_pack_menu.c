/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_menu.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_time.v.h"

struct AMenu {
    AMenuState state;
    AList* items;
    void* selectedItem;
    unsigned selectedIndex;
    ASample* soundAccept;
    ASample* soundCancel;
    ASample* soundBrowse;
    AInputButton* next;
    AInputButton* back;
    AInputButton* select;
    AInputButton* cancel;
};

AMenu* a_menu_new(AInputButton* Next, AInputButton* Back, AInputButton* Select, AInputButton* Cancel)
{
    AMenu* m = a_mem_malloc(sizeof(AMenu));

    m->state = A_MENU_STATE_RUNNING;
    m->items = a_list_new();
    m->selectedItem = NULL;
    m->selectedIndex = 0;
    m->soundAccept = NULL;
    m->soundCancel = NULL;
    m->soundBrowse = NULL;
    m->next = a_button_dup(Next);
    m->back = a_button_dup(Back);
    m->select = Select;
    m->cancel = Cancel;

    a_button_pressSetRepeat(m->next, a_time_msToTicks(200));
    a_button_pressSetRepeat(m->back, a_time_msToTicks(200));

    return m;
}

void a_menu_free(AMenu* Menu)
{
    a_menu_freeEx(Menu, NULL);
}

void a_menu_freeEx(AMenu* Menu, AFree* ItemFree)
{
    a_list_freeEx(Menu->items, ItemFree);
    a_button_free(Menu->next);
    a_button_free(Menu->back);
    free(Menu);
}

void a_menu_soundSet(AMenu* Menu, ASample* Accept, ASample* Cancel, ASample* Browse)
{
    Menu->soundAccept = Accept;
    Menu->soundCancel = Cancel;
    Menu->soundBrowse = Browse;
}

void a_menu_itemAdd(AMenu* Menu, void* Item)
{
    a_list_addLast(Menu->items, Item);

    if(Menu->selectedItem == NULL) {
        Menu->selectedItem = Item;
    }
}

void a_menu_tick(AMenu* Menu)
{
    if(a_list_isEmpty(Menu->items) || Menu->state != A_MENU_STATE_RUNNING) {
        return;
    }

    bool browsed = false;

    if(a_button_pressGet(Menu->back)) {
        browsed = true;

        if(Menu->selectedIndex-- == 0) {
            Menu->selectedIndex = a_list_sizeGet(Menu->items) - 1;
        }
    } else if(a_button_pressGet(Menu->next)) {
        browsed = true;

        if(++Menu->selectedIndex == a_list_sizeGet(Menu->items)) {
            Menu->selectedIndex = 0;
        }
    }

    if(browsed) {
        Menu->selectedItem = a_list_getIndex(Menu->items, Menu->selectedIndex);

        if(Menu->soundBrowse) {
            a_channel_play(A_CHANNEL_ANY, Menu->soundBrowse, A_CHANNEL_NORMAL);
        }
    } else {
        if(a_button_pressGet(Menu->select)) {
            Menu->state = A_MENU_STATE_SELECTED;

            if(Menu->soundAccept) {
                a_channel_play(
                    A_CHANNEL_ANY, Menu->soundAccept, A_CHANNEL_NORMAL);
            }
        } else if(Menu->cancel && a_button_pressGet(Menu->cancel)) {
            Menu->state = A_MENU_STATE_CANCELED;

            if(Menu->soundCancel) {
                a_channel_play(
                    A_CHANNEL_ANY, Menu->soundCancel, A_CHANNEL_NORMAL);
            }
        }
    }

    if(Menu->state != A_MENU_STATE_RUNNING) {
        a_button_pressClear(Menu->next);
        a_button_pressClear(Menu->back);
        a_button_pressClear(Menu->select);

        if(Menu->cancel) {
            a_button_pressClear(Menu->cancel);
        }
    }
}

AMenuState a_menu_stateGet(const AMenu* Menu)
{
    return Menu->state;
}

AList* a_menu_itemsListGet(const AMenu* Menu)
{
    return Menu->items;
}

bool a_menu_itemIsSelected(const AMenu* Menu, const void* Item)
{
    return Item == Menu->selectedItem;
}

unsigned a_menu_selectedIndexGet(const AMenu* Menu)
{
    return Menu->selectedIndex;
}

void* a_menu_itemGetSelected(const AMenu* Menu)
{
    return Menu->selectedItem;
}

void a_menu_keepRunning(AMenu* Menu)
{
    Menu->state = A_MENU_STATE_RUNNING;
}

void a_menu_reset(AMenu* Menu)
{
    Menu->state = A_MENU_STATE_RUNNING;
    Menu->selectedItem = a_list_getFirst(Menu->items);
    Menu->selectedIndex = 0;
}
