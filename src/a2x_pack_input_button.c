/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

struct AInputButton {
    AInputUserHeader header;
    AList* combos; // List of lists of AInputButtonSource; for combo buttons
    ATimer* autoRepeat;
    AListNode* buttonsListNode;
    bool isClone;
};

struct AInputButtonSource {
    AInputSourceHeader header;
    AList* forwardButtons; // List of AInputButtonSource
    bool pressed;
    bool ignorePressed;
};

static AList* g_buttons;
static AStrHash* g_sourceButtons;
static AList* g_pressQueue;
static AList* g_releaseQueue;

static void a_button__free(AInputButton* Button);

void a_input_button__init(void)
{
    g_buttons = a_list_new();
    g_sourceButtons = a_strhash_new();
    g_pressQueue = a_list_new();
    g_releaseQueue = a_list_new();
}

void a_input_button__uninit(void)
{
    A_LIST_ITERATE(g_buttons, AInputButton*, b) {
        a_button__free(b);
    }

    A_STRHASH_ITERATE(g_sourceButtons, AInputButtonSource*, b) {
        a_input_button__freeSource(b);
    }

    a_list_free(g_buttons);
    a_strhash_free(g_sourceButtons);
    a_list_free(g_pressQueue);
    a_list_free(g_releaseQueue);
}

AInputButtonSource* a_input_button__newSource(const char* Name, const char* Id)
{
    AInputButtonSource* b = a_mem_malloc(sizeof(AInputButtonSource));

    a_input__initSourceHeader(&b->header, Name);

    b->forwardButtons = a_list_new();
    b->pressed = false;
    b->ignorePressed = false;

    if(a_input_numControllers() == 0) {
        // Keys are declared before controllers are created
        a_strhash_add(g_sourceButtons, Id, b);
    }

    return b;
}

void a_input_button__freeSource(AInputButtonSource* Button)
{
    a_list_free(Button->forwardButtons);
    a_input__freeSourceHeader(&Button->header);
}

void a_input_button__forwardToButton(AInputButtonSource* Button, AInputButtonSource* Binding)
{
    a_list_addLast(Button->forwardButtons, Binding);
}

AInputButton* a_button_new(const char* Ids)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    a_input__initUserHeader(&b->header);

    b->combos = a_list_new();
    b->autoRepeat = NULL;
    b->buttonsListNode = a_list_addLast(g_buttons, b);
    b->isClone = false;

    AStrTok* tok = a_strtok_new(Ids, ", ");

    A_STRTOK_ITERATE(tok, id) {
        if(a_str_firstIndex(id, '+') > 0) {
            AList* combo = a_list_new();
            AStrTok* tok = a_strtok_new(id, "+");
            bool missing = false;

            A_STRTOK_ITERATE(tok, part) {
                AInputButtonSource* button = a_strhash_get(g_sourceButtons,
                                                           part);

                if(button == NULL) {
                    button = a_controller__getButton(part);

                    if(button == NULL) {
                        missing = true;
                        break;
                    }
                }

                a_list_addLast(combo, button);
            }

            if(missing || a_list_empty(combo)) {
                a_list_free(combo);
            } else {
                a_list_addLast(b->combos, combo);
            }

            a_strtok_free(tok);
        } else {
            a_input__findSourceInput(g_sourceButtons,
                                     a_controller__getButtonCollection(),
                                     id,
                                     &b->header);
        }
    }

    a_strtok_free(tok);

    if(!a_list_empty(b->header.sourceInputs)) {
        AInputButtonSource* btn = a_list_getLast(b->header.sourceInputs);
        b->header.name = a_str_dup(btn->header.name);
    } else if(!a_list_empty(b->combos)) {
        AStrBuilder* sb = a_strbuilder_new(128);
        AList* combo = a_list_getLast(b->combos);

        A_LIST_ITERATE(combo, AInputButtonSource*, button) {
            a_strbuilder_addString(sb, button->header.name);

            if(!A_LIST_IS_LAST()) {
                a_strbuilder_addString(sb, "+");
            }
        }

        b->header.name = a_str_dup(a_strbuilder_string(sb));
        a_strbuilder_free(sb);
    } else {
        a_out__error("No buttons found for '%s'", Ids);
        b->header.name = a_str_dup("<none>");
    }

    return b;
}

AInputButton* a_button_dup(const AInputButton* Button)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    *b = *Button;
    b->autoRepeat = NULL;
    b->buttonsListNode = a_list_addLast(g_buttons, b);
    b->isClone = true;

    return b;
}

void a_button__free(AInputButton* Button)
{
    if(!Button->isClone) {
        A_LIST_ITERATE(Button->combos, AList*, andList) {
            a_list_free(andList);
        }

        a_list_free(Button->combos);
        a_input__freeUserHeader(&Button->header);
    }

    if(Button->autoRepeat) {
        a_timer_free(Button->autoRepeat);
    }

    free(Button);
}

void a_button_free(AInputButton* Button)
{
    a_list_removeNode(Button->buttonsListNode);
    a_button__free(Button);
}

bool a_button_working(const AInputButton* Button)
{
    return !a_list_empty(Button->header.sourceInputs)
        || !a_list_empty(Button->combos);
}

const char* a_button_name(const AInputButton* Button)
{
    return Button->header.name;
}

bool a_button_get(AInputButton* Button)
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
            if(!a_timer_running(Button->autoRepeat)
                || a_timer_expired(Button->autoRepeat)) {

                a_timer_start(Button->autoRepeat);
            } else {
                pressed = false;
            }
        } else {
            a_timer_stop(Button->autoRepeat);
        }
    }

    return pressed;
}

void a_button_release(const AInputButton* Button)
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

bool a_button_getOnce(AInputButton* Button)
{
    bool pressed = a_button_get(Button);

    if(pressed) {
        a_button_release(Button);
    }

    return pressed;
}

void a_button_setRepeat(AInputButton* Button, unsigned RepeatFrames)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = a_timer_new(A_TIMER_FRAMES, RepeatFrames);
    } else {
        a_timer_setPeriod(Button->autoRepeat, RepeatFrames);
        a_timer_stop(Button->autoRepeat);
    }
}

void a_input_button__setState(AInputButtonSource* Button, bool Pressed)
{
    if(!Pressed && Button->ignorePressed) {
        Button->ignorePressed = false;
    }

    Button->pressed = Pressed;

    a_input__setFreshEvent(&Button->header);

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
        a_input_button__setState(b, true);
    }

    A_LIST_ITERATE(g_releaseQueue, AInputButtonSource*, b) {
        // Only release if hadn't just received a press
        if(!a_input__hasFreshEvent(&b->header)) {
            a_input_button__setState(b, false);
        }
    }

    a_list_clear(g_pressQueue);
    a_list_clear(g_releaseQueue);
}
