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
    bool isLeaf;
    union {
        struct {
            AList* orList; // List of AInputButtonSource; for alias buttons
        } node;
        struct {
            bool pressed;
            bool ignorePressed;
            AList* forwardButtons; // List of AInputButtonSource
        } leaf;
    } u;
};

static AList* g_buttons;
static AStrHash* g_sourceButtons;
static AList* g_pressQueue;
static AList* g_releaseQueue;

static void newSourceNode(const char* Name, const char* Id, const char* ButtonNames);
static void a_button__free(AInputButton* Button);

void a_input_button__init(void)
{
    g_buttons = a_list_new();
    g_sourceButtons = a_strhash_new();
    g_pressQueue = a_list_new();
    g_releaseQueue = a_list_new();
}

void a_input_button__init2(void)
{
    // IDs are in order of increasing platform-specificity
    newSourceNode("Up", "generic.up", "key.up gamepad.b.up gp2x.up wiz.up caanoo.up pandora.up");
    newSourceNode("Down", "generic.down", "key.down gamepad.b.down gp2x.down wiz.down caanoo.down pandora.down");
    newSourceNode("Left", "generic.left", "key.left gamepad.b.left gp2x.left wiz.left caanoo.left pandora.left");
    newSourceNode("Right", "generic.right", "key.right gamepad.b.right gp2x.right wiz.right caanoo.right pandora.right");

    newSourceNode("B0", "generic.b0", "key.z gamepad.b.0 gamepad.b.a gp2x.x wiz.x caanoo.x pandora.x");
    newSourceNode("B1", "generic.b1", "key.x gamepad.b.1 gamepad.b.b gp2x.b wiz.b caanoo.b pandora.b");
    newSourceNode("B2", "generic.b2", "key.c gamepad.b.2 gamepad.b.x gp2x.a wiz.a caanoo.a pandora.a");
    newSourceNode("B3", "generic.b3", "key.v gamepad.b.3 gamepad.b.y gp2x.y wiz.y caanoo.y pandora.y");
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

    b->isLeaf = true;
    b->u.leaf.pressed = false;
    b->u.leaf.ignorePressed = false;
    b->u.leaf.forwardButtons = a_list_new();

    if(a_input_numControllers() == 0) {
        // Keys are declared before controllers are created
        a_strhash_add(g_sourceButtons, Id, b);
    }

    return b;
}

static void newSourceNode(const char* Name, const char* Id, const char* ButtonIds)
{
    AInputButtonSource* b = a_mem_malloc(sizeof(AInputButtonSource));

    a_input__initSourceHeader(&b->header, Name);

    b->isLeaf = false;
    b->u.node.orList = a_list_new();

    AStrTok* tok = a_strtok_new(ButtonIds, ", ");

    A_STRTOK_ITERATE(tok, id) {
        AInputButtonSource* btn = a_strhash_get(g_sourceButtons, id);

        if(btn == NULL) {
            btn = a_controller__getButton(id);
        }

        if(btn != NULL) {
            a_list_addLast(b->u.node.orList, btn);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(b->u.node.orList)) {
        a_out__fatal("'%s' found no buttons in '%s'", Id, ButtonIds);
    }

    a_strhash_add(g_sourceButtons, Id, b);
}

static const char* getButtonName(AInputButtonSource* Button)
{
    if(Button->isLeaf) {
        return Button->header.name;
    } else {
        return getButtonName(a_list_getLast(Button->u.node.orList));
    }
}

void a_input_button__freeSource(AInputButtonSource* Button)
{
    if(Button->isLeaf) {
        a_list_free(Button->u.leaf.forwardButtons);
    } else {
        a_list_free(Button->u.node.orList);
    }

    a_input__freeSourceHeader(&Button->header);
}

void a_input_button__forwardTo(AInputButtonSource* Button, AInputButtonSource* Binding)
{
    a_list_addLast(Button->u.leaf.forwardButtons, Binding);
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
        b->header.name = a_str_dup(
            getButtonName(a_list_getLast(b->header.sourceInputs)));
    } else if(!a_list_empty(b->combos)) {
        AStrBuilder* sb = a_strbuilder_new(128);
        AList* combo = a_list_getLast(b->combos);

        A_LIST_ITERATE(combo, AInputButtonSource*, button) {
            a_strbuilder_addString(sb, getButtonName(button));

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

AInputButton* a_button_clone(const AInputButton* Button)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    *b = *Button;
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

static bool isSourceButtonPressed(const AInputButtonSource* Button)
{
    if(Button->isLeaf) {
        return Button->u.leaf.pressed && !Button->u.leaf.ignorePressed;
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputButtonSource*, b) {
            if(isSourceButtonPressed(b)) {
                return true;
            }
        }

        return false;
    }
}

static void releaseSourceButton(AInputButtonSource* Button)
{
    if(Button->isLeaf) {
        if(Button->u.leaf.pressed) {
            Button->u.leaf.ignorePressed = true;
        }
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputButtonSource*, b) {
            releaseSourceButton(b);
        }
    }
}

bool a_button_get(AInputButton* Button)
{
    bool pressed = false;

    A_LIST_ITERATE(Button->header.sourceInputs, AInputButtonSource*, b) {
        if(isSourceButtonPressed(b)) {
            pressed = true;
            goto done;
        }
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputButtonSource*, b) {
            if(!isSourceButtonPressed(b)) {
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
        releaseSourceButton(b);
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputButtonSource*, b) {
            releaseSourceButton(b);
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
    if(!Pressed && Button->u.leaf.ignorePressed) {
        Button->u.leaf.ignorePressed = false;
    }

    Button->u.leaf.pressed = Pressed;

    a_input__setFreshEvent(&Button->header);

    A_LIST_ITERATE(Button->u.leaf.forwardButtons, AInputButtonSource*, b) {
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
