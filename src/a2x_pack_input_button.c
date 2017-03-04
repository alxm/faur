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
    AInputHeader header;
    AList* combos; // List of lists of AInputSourceButton; for combo buttons
    unsigned repeatFrames;
    unsigned lastPressedFrame;
};

struct AInputSourceButton {
    AInputSourceHeader header;
    bool isLeaf;
    union {
        struct {
            AList* orList; // List of AInputSourceButton; for alias buttons
        } node;
        struct {
            bool pressed;
            bool ignorePressed;
            AList* buttonBindings; // List of AInputSourceButton
        } leaf;
    } u;
};

static AList* g_buttons;
static AStrHash* g_sourceButtons;
static AList* g_pressQueue;
static AList* g_releaseQueue;

static void a_input__newSourceButtonNode(const char* Name, const char* ButtonNames);

void a_input_button__init(void)
{
    g_buttons = a_list_new();
    g_sourceButtons = a_strhash_new();
    g_pressQueue = a_list_new();
    g_releaseQueue = a_list_new();
}

void a_input_button__init2(void)
{
    a_input__newSourceButtonNode("generic.up", "key.up controller.up gp2x.up wiz.up caanoo.up pandora.up");
    a_input__newSourceButtonNode("generic.down", "key.down controller.down gp2x.down wiz.down caanoo.down pandora.down");
    a_input__newSourceButtonNode("generic.left", "key.left controller.left gp2x.left wiz.left caanoo.left pandora.left");
    a_input__newSourceButtonNode("generic.right", "key.right controller.right gp2x.right wiz.right caanoo.right pandora.right");

    a_input__newSourceButtonNode("generic.b0", "key.z controller.b0 gp2x.x wiz.x caanoo.x pandora.x");
    a_input__newSourceButtonNode("generic.b1", "key.x controller.b1 gp2x.b wiz.b caanoo.b pandora.b");
    a_input__newSourceButtonNode("generic.b2", "key.c controller.b2 gp2x.a wiz.a caanoo.a pandora.a");
    a_input__newSourceButtonNode("generic.b3", "key.v controller.b3 gp2x.y wiz.y caanoo.y pandora.y");
}

void a_input_button__uninit(void)
{
    A_LIST_ITERATE(g_buttons, AInputButton*, b) {
        A_LIST_ITERATE(b->combos, AList*, andList) {
            a_list_free(andList);
        }

        a_list_free(b->combos);
        a_input__freeHeader(&b->header);
        free(b);
    }

    A_STRHASH_ITERATE(g_sourceButtons, AInputSourceButton*, b) {
        a_input__freeSourceButton(b);
    }

    a_list_free(g_buttons);
    a_strhash_free(g_sourceButtons);
    a_list_free(g_pressQueue);
    a_list_free(g_releaseQueue);
}

AInputSourceButton* a_input__newSourceButton(const char* Name)
{
    AInputSourceButton* b = a_mem_malloc(sizeof(AInputSourceButton));

    a_input__initSourceHeader(&b->header, Name);

    b->isLeaf = true;
    b->u.leaf.pressed = false;
    b->u.leaf.ignorePressed = false;
    b->u.leaf.buttonBindings = a_list_new();

    if(a_input_numControllers() == 0) {
        // Keys are declared before controllers are created
        a_strhash_add(g_sourceButtons, Name, b);
    }

    return b;
}

static void a_input__newSourceButtonNode(const char* Name, const char* ButtonNames)
{
    AInputSourceButton* b = a_mem_malloc(sizeof(AInputSourceButton));

    a_input__initSourceHeader(&b->header, Name);

    b->isLeaf = false;
    b->u.node.orList = a_list_new();

    AStrTok* tok = a_strtok_new(ButtonNames, ", ");

    A_STRTOK_ITERATE(tok, name) {
        AInputSourceButton* btn = a_strhash_get(g_sourceButtons, name);

        if(btn == NULL) {
            btn = a_controller__getButton(name);
        }

        if(btn != NULL) {
            a_list_addLast(b->u.node.orList, btn);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(b->u.node.orList)) {
        a_out__fatal("'%s' found no buttons in '%s'", Name, ButtonNames);
    }

    a_strhash_add(g_sourceButtons, Name, b);
}

void a_input__freeSourceButton(AInputSourceButton* Button)
{
    if(Button->isLeaf) {
        a_list_free(Button->u.leaf.buttonBindings);
    } else {
        a_list_free(Button->u.node.orList);
    }

    a_input__freeSourceHeader(&Button->header);
}

void a_input__buttonButtonBinding(AInputSourceButton* Button,AInputSourceButton* Binding)
{
    a_list_addLast(Button->u.leaf.buttonBindings, Binding);
}

AInputButton* a_button_new(const char* Names)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    a_input__initHeader(&b->header);

    b->combos = a_list_new();
    b->repeatFrames = 0;
    b->lastPressedFrame = 0;

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        if(a_str_firstIndex(name, '+') > 0) {
            AList* buttons = a_list_new();
            AStrTok* tok = a_strtok_new(name, "+");
            bool missing = false;

            A_STRTOK_ITERATE(tok, part) {
                AInputSourceButton* button = a_strhash_get(g_sourceButtons,
                                                           part);

                if(button == NULL) {
                    button = a_controller__getButton(part);

                    if(button == NULL) {
                        missing = true;
                        a_list_free(buttons);
                        break;
                    }
                }

                a_list_addLast(buttons, button);
            }

            if(!missing) {
                a_list_addLast(b->combos, buttons);

                if(b->header.name == NULL) {
                    AStrBuilder* sb = a_strbuilder_new(128);

                    A_LIST_ITERATE(buttons, AInputSourceButton*, button) {
                        a_strbuilder_addString(sb, button->header.shortName);

                        if(!A_LIST_IS_LAST()) {
                            a_strbuilder_addString(sb, "+");
                        }
                    }

                    b->header.name = a_str_dup(a_strbuilder_string(sb));

                    a_strbuilder_free(sb);
                }
            }

            a_strtok_free(tok);
        } else {
            a_input__findSourceInput(name,
                                     g_sourceButtons,
                                     a_controller__getButtonsCollection(),
                                     &b->header);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(b->header.sourceInputs) && a_list_empty(b->combos)) {
        a_out__error("No buttons found for '%s'", Names);
    }

    a_list_addLast(g_buttons, b);

    return b;
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

static bool isSourceButtonPressed(const AInputSourceButton* Button)
{
    if(Button->isLeaf) {
        return Button->u.leaf.pressed && !Button->u.leaf.ignorePressed;
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputSourceButton*, b) {
            if(isSourceButtonPressed(b)) {
                return true;
            }
        }

        return false;
    }
}

static void releaseSourceButton(AInputSourceButton* Button)
{
    if(Button->isLeaf) {
        if(Button->u.leaf.pressed) {
            Button->u.leaf.ignorePressed = true;
        }
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputSourceButton*, b) {
            releaseSourceButton(b);
        }
    }
}

bool a_button_get(AInputButton* Button)
{
    const unsigned now = a_fps_getCounter();

    A_LIST_ITERATE(Button->header.sourceInputs, AInputSourceButton*, b) {
        if(isSourceButtonPressed(b)) {
            if(Button->repeatFrames > 0) {
                if(now - Button->lastPressedFrame >= Button->repeatFrames) {
                    Button->lastPressedFrame = now;
                    return true;
                }
            } else {
                return true;
            }
        }
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputSourceButton*, b) {
            if(!isSourceButtonPressed(b)) {
                break;
            }

            if(Button->repeatFrames > 0
                && now - Button->lastPressedFrame < Button->repeatFrames) {

                break;
            }

            if(A_LIST_IS_LAST()) {
                if(Button->repeatFrames > 0) {
                    Button->lastPressedFrame = now;
                }

                return true;
            }
        }
    }

    return false;
}

void a_button_release(const AInputButton* Button)
{
    A_LIST_ITERATE(Button->header.sourceInputs, AInputSourceButton*, b) {
        releaseSourceButton(b);
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputSourceButton*, b) {
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
    Button->repeatFrames = RepeatFrames;
    Button->lastPressedFrame = a_fps_getCounter() - RepeatFrames;
}

void a_input__button_setState(AInputSourceButton* Button, bool Pressed)
{
    if(!Pressed && Button->u.leaf.ignorePressed) {
        Button->u.leaf.ignorePressed = false;
    }

    Button->u.leaf.pressed = Pressed;

    a_input__setFreshEvent(&Button->header);

    A_LIST_ITERATE(Button->u.leaf.buttonBindings, AInputSourceButton*, b) {
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
    A_LIST_ITERATE(g_pressQueue, AInputSourceButton*, b) {
        // Overwrite whatever current state with a press
        a_input__button_setState(b, true);
    }

    A_LIST_ITERATE(g_releaseQueue, AInputSourceButton*, b) {
        // Only release if hadn't just received a press
        if(!a_input__hasFreshEvent(&b->header)) {
            a_input__button_setState(b, false);
        }
    }

    a_list_clear(g_pressQueue);
    a_list_clear(g_releaseQueue);
}
