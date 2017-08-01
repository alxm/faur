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

#include "a2x_pack_menu.v.h"

struct AMenu {
    AMenuState state;
    AList* items;
    void* selectedItem;
    unsigned selectedIndex;
    ASound* soundAccept;
    ASound* soundCancel;
    ASound* soundBrowse;
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

    a_button_setRepeat(m->next, a_fps_msToFrames(200));
    a_button_setRepeat(m->back, a_fps_msToFrames(200));

    return m;
}

void a_menu_free(AMenu* Menu)
{
    a_menu_freeEx(Menu, NULL);
}

void a_menu_freeEx(AMenu* Menu, AListFree* ItemFree)
{
    a_list_freeEx(Menu->items, ItemFree);
    a_button_free(Menu->next);
    a_button_free(Menu->back);
    free(Menu);
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

void a_menu_handleInput(AMenu* Menu)
{
    if(a_list_isEmpty(Menu->items) || Menu->state != A_MENU_STATE_RUNNING) {
        return;
    }

    bool browsed = false;

    if(a_button_getPressed(Menu->back)) {
        browsed = true;

        if(Menu->selectedIndex-- == 0) {
            Menu->selectedIndex = a_list_getSize(Menu->items) - 1;
        }
    } else if(a_button_getPressed(Menu->next)) {
        browsed = true;

        if(++Menu->selectedIndex == a_list_getSize(Menu->items)) {
            Menu->selectedIndex = 0;
        }
    }

    if(browsed) {
        Menu->selectedItem = a_list_getIndex(Menu->items, Menu->selectedIndex);

        if(Menu->soundBrowse) {
            a_sfx_play(Menu->soundBrowse);
        }
    } else {
        if(a_button_getPressed(Menu->select)) {
            Menu->state = A_MENU_STATE_SELECTED;

            if(Menu->soundAccept) {
                a_sfx_play(Menu->soundAccept);
            }
        } else if(Menu->cancel && a_button_getPressed(Menu->cancel)) {
            Menu->state = A_MENU_STATE_CANCELED;

            if(Menu->soundCancel) {
                a_sfx_play(Menu->soundCancel);
            }
        }
    }

    if(Menu->state != A_MENU_STATE_RUNNING) {
        a_button_release(Menu->next);
        a_button_release(Menu->back);
        a_button_release(Menu->select);

        if(Menu->cancel) {
            a_button_release(Menu->cancel);
        }
    }
}

AMenuState a_menu_getState(const AMenu* Menu)
{
    return Menu->state;
}

AList* a_menu_getItems(const AMenu* Menu)
{
    return Menu->items;
}

bool a_menu_isItemSelected(const AMenu* Menu, const void* Item)
{
    return Item == Menu->selectedItem;
}

unsigned a_menu_getSelectedIndex(const AMenu* Menu)
{
    return Menu->selectedIndex;
}

void* a_menu_getSelectedItem(const AMenu* Menu)
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
