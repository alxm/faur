/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_input_button.v.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_input_controller.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strbuilder.v.h"
#include "a2x_pack_timer.v.h"

struct AInputButton {
    AInputUserHeader header;
    AList* combos; // List of lists of AInputButtonSource; for combo buttons
    ATimer* autoRepeat;
    bool isClone;
};

struct AInputButtonSource {
    AInputSourceHeader header;
    AList* forwardButtons; // List of AInputButtonSource
    bool pressed;
    bool ignorePressed;
};

static AStrHash* g_keys;
static AList* g_pressQueue;
static AList* g_releaseQueue;

void a_input_button__init(void)
{
    g_keys = a_strhash_new();
    g_pressQueue = a_list_new();
    g_releaseQueue = a_list_new();
}

void a_input_button__uninit(void)
{
    a_strhash_freeEx(g_keys, (AFree*)a_input_button__freeSource);
    a_list_free(g_pressQueue);
    a_list_free(g_releaseQueue);
}

AInputButtonSource* a_input_button__newSource(const char* Name, const char* Id)
{
    AInputButtonSource* b = a_mem_malloc(sizeof(AInputButtonSource));

    a_input__sourceHeaderInit(&b->header, Name);

    b->forwardButtons = a_list_new();
    b->pressed = false;
    b->ignorePressed = false;

    if(a_input_controllerNumGet() == 0) {
        // Keys are declared before controllers are created
        a_strhash_add(g_keys, Id, b);
    }

    return b;
}

void a_input_button__freeSource(AInputButtonSource* Button)
{
    a_list_free(Button->forwardButtons);
    a_input__sourceHeaderFree(&Button->header);
}

AInputButtonSource* a_input_button__keyGet(const char* Id)
{
    return a_strhash_get(g_keys, Id);
}

void a_input_button__forwardToButton(AInputButtonSource* Button, AInputButtonSource* Binding)
{
    a_list_addLast(Button->forwardButtons, Binding);
}

AInputButton* a_button_new(const char* Ids)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    a_input__userHeaderInit(&b->header);

    b->combos = a_list_new();
    b->autoRepeat = NULL;
    b->isClone = false;

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        if(a_str_indexGetFirst(id, '+') > 0) {
            AList* combo = a_list_new();
            AList* tok = a_str_split(id, "+");
            bool missing = false;

            A_LIST_ITERATE(tok, char*, part) {
                AInputButtonSource* button = a_strhash_get(g_keys, part);

                if(button == NULL) {
                    button = a_controller__buttonGet(part);

                    if(button == NULL) {
                        missing = true;
                        break;
                    }
                }

                a_list_addLast(combo, button);
            }

            if(missing || a_list_isEmpty(combo)) {
                a_list_free(combo);
            } else {
                a_list_addLast(b->combos, combo);
            }

            a_list_freeEx(tok, free);
        } else {
            a_input__userHeaderFindSource(
                &b->header, id, g_keys, a_controller__buttonCollectionGet());
        }
    }

    a_list_freeEx(tok, free);

    if(!a_list_isEmpty(b->header.sourceInputs)) {
        AInputButtonSource* btn = a_list_getLast(b->header.sourceInputs);
        b->header.name = a_str_dup(btn->header.name);
    } else if(!a_list_isEmpty(b->combos)) {
        AStrBuilder* sb = a_strbuilder_new(128);
        AList* combo = a_list_getLast(b->combos);

        A_LIST_ITERATE(combo, AInputButtonSource*, button) {
            a_strbuilder_add(sb, button->header.name);

            if(!A_LIST_IS_LAST()) {
                a_strbuilder_add(sb, "+");
            }
        }

        b->header.name = a_str_dup(a_strbuilder_get(sb));
        a_strbuilder_free(sb);
    } else {
        a_out__error("a_button_new: No buttons found for '%s'", Ids);
        b->header.name = a_str_dup("<none>");
    }

    return b;
}

AInputButton* a_button_dup(const AInputButton* Button)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    *b = *Button;
    b->autoRepeat = NULL;
    b->isClone = true;

    return b;
}

void a_button_free(AInputButton* Button)
{
    if(Button == NULL) {
        return;
    }

    if(!Button->isClone) {
        a_list_freeEx(Button->combos, (AFree*)a_list_free);
        a_input__userHeaderFree(&Button->header);
    }

    a_timer_free(Button->autoRepeat);

    free(Button);
}

bool a_button_isWorking(const AInputButton* Button)
{
    return !a_list_isEmpty(Button->header.sourceInputs)
        || !a_list_isEmpty(Button->combos);
}

const char* a_button_nameGet(const AInputButton* Button)
{
    return Button->header.name;
}

bool a_button_pressGet(AInputButton* Button)
{
    bool pressed = false;

    A_LIST_ITERATE(Button->header.sourceInputs, AInputButtonSource*, b) {
        if(b->pressed && !b->ignorePressed) {
            pressed = true;
            goto done;
        }
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputButtonSource*, b) {
            if(!b->pressed || b->ignorePressed) {
                break;
            } else if(A_LIST_IS_LAST()) {
                pressed = true;
                goto done;
            }
        }
    }

done:
    if(Button->autoRepeat) {
        if(pressed) {
            if(!a_timer_isRunning(Button->autoRepeat)) {
                a_timer_start(Button->autoRepeat);
            } else if(!a_timer_isExpired(Button->autoRepeat)) {
                pressed = false;
            }
        } else {
            a_timer_stop(Button->autoRepeat);
        }
    }

    return pressed;
}

bool a_button_pressGetOnce(AInputButton* Button)
{
    bool pressed = a_button_pressGet(Button);

    if(pressed) {
        a_button_pressClear(Button);
    }

    return pressed;
}

void a_button_pressSetRepeat(AInputButton* Button, unsigned RepeatTicks)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = a_timer_new(A_TIMER_TICKS, RepeatTicks, true);
    } else {
        a_timer_stop(Button->autoRepeat);
        a_timer_periodSet(Button->autoRepeat, RepeatTicks);
    }
}

void a_button_pressClear(const AInputButton* Button)
{
    A_LIST_ITERATE(Button->header.sourceInputs, AInputButtonSource*, b) {
        if(b->pressed) {
            b->ignorePressed = true;
        }
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputButtonSource*, b) {
            if(b->pressed) {
                b->ignorePressed = true;
            }
        }
    }
}

void a_input_button__stateSet(AInputButtonSource* Button, bool Pressed)
{
    if(!Pressed && Button->ignorePressed) {
        Button->ignorePressed = false;
    }

    Button->pressed = Pressed;

    a_input__freshEventSet(&Button->header);

    A_LIST_ITERATE(Button->forwardButtons, AInputButtonSource*, b) {
        // Queue forwarded button presses and releases to be processed after
        // all input events were received, so they don't conflict with them.
        if(Pressed) {
            a_list_addLast(g_pressQueue, b);
        } else {
            a_list_addLast(g_releaseQueue, b);
        }
    }
}

void a_input_button__processQueue(void)
{
    A_LIST_ITERATE(g_pressQueue, AInputButtonSource*, b) {
        // Overwrite whatever current state with a press
        a_input_button__stateSet(b, true);
    }

    A_LIST_ITERATE(g_releaseQueue, AInputButtonSource*, b) {
        // Only release if hadn't just received a press
        if(!a_input__freshEventGet(&b->header)) {
            a_input_button__stateSet(b, false);
        }
    }

    a_list_clear(g_pressQueue);
    a_list_clear(g_releaseQueue);
}
