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
        } leaf;
    } u;
};

static AList* g_buttons;
static AStrHash* g_sourceButtons;

static void a_input__newSourceButtonNode(const char* Name, const char* ButtonNames);

void a_input_button__init(void)
{
    g_buttons = a_list_new();
    g_sourceButtons = a_strhash_new();
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
        if(!b->isLeaf) {
            a_list_free(b->u.node.orList);
        }

        a_input__freeSourceHeader(&b->header);
    }

    a_list_free(g_buttons);
    a_strhash_free(g_sourceButtons);
}

AInputSourceButton* a_input__newSourceButton(const char* Name)
{
    AInputSourceButton* b = a_mem_malloc(sizeof(AInputSourceButton));

    a_input__initSourceHeader(&b->header, Name);

    b->isLeaf = true;
    b->u.leaf.pressed = false;
    b->u.leaf.ignorePressed = false;

    if(a_input__activeController == NULL) {
        a_strhash_add(g_sourceButtons, Name, b);
    } else {
        a_strhash_add(a_input__activeController->buttons, Name, b);
    }

    return b;
}

AInputSourceButton* a_input__getSourceButton(const char* Name)
{
    return a_strhash_get(g_sourceButtons, Name);
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

        if(btn == NULL && a_input__activeController != NULL) {
            btn = a_strhash_get(a_input__activeController->buttons, name);
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
                    if(a_input__activeController != NULL) {
                        button = a_strhash_get(a_input__activeController->buttons,
                                               part);
                    }

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
        } else if(!a_input__findSourceInput(name, g_sourceButtons, &b->header)) {
            if(a_input__activeController != NULL) {
                a_input__findSourceInput(name, a_input__activeController->buttons, &b->header);
            }
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
}

void a_input_button__adjust(void)
{
    // GP2X and Wiz dpad diagonals show up as dedicated buttons instead of a
    // combination of two separate buttons. This code checks diagonal events
    // and sets the state of each actual button accordingly.
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            AInputSourceButton* upLeft = a_strhash_get(g_sourceButtons, "gp2x.upleft");
            AInputSourceButton* upRight = a_strhash_get(g_sourceButtons, "gp2x.upright");
            AInputSourceButton* downLeft = a_strhash_get(g_sourceButtons, "gp2x.downleft");
            AInputSourceButton* downRight = a_strhash_get(g_sourceButtons, "gp2x.downright");
            AInputSourceButton* up = a_strhash_get(g_sourceButtons, "gp2x.up");
            AInputSourceButton* down = a_strhash_get(g_sourceButtons, "gp2x.down");
            AInputSourceButton* left = a_strhash_get(g_sourceButtons, "gp2x.left");
            AInputSourceButton* right = a_strhash_get(g_sourceButtons, "gp2x.right");
        #elif A_PLATFORM_WIZ
            AInputSourceButton* upLeft = a_strhash_get(g_sourceButtons, "wiz.upleft");
            AInputSourceButton* upRight = a_strhash_get(g_sourceButtons, "wiz.upright");
            AInputSourceButton* downLeft = a_strhash_get(g_sourceButtons, "wiz.downleft");
            AInputSourceButton* downRight = a_strhash_get(g_sourceButtons, "wiz.downright");
            AInputSourceButton* up = a_strhash_get(g_sourceButtons, "wiz.up");
            AInputSourceButton* down = a_strhash_get(g_sourceButtons, "wiz.down");
            AInputSourceButton* left = a_strhash_get(g_sourceButtons, "wiz.left");
            AInputSourceButton* right = a_strhash_get(g_sourceButtons, "wiz.right");
        #endif

        if(a_input__hasFreshEvent(&upLeft->header)) {
            a_input__button_setState(up, upLeft->u.leaf.pressed);
            a_input__button_setState(left, upLeft->u.leaf.pressed);
        }

        if(a_input__hasFreshEvent(&upRight->header)) {
            a_input__button_setState(up, upRight->u.leaf.pressed);
            a_input__button_setState(right, upRight->u.leaf.pressed);
        }

        if(a_input__hasFreshEvent(&downLeft->header)) {
            a_input__button_setState(down, downLeft->u.leaf.pressed);
            a_input__button_setState(left, downLeft->u.leaf.pressed);
        }

        if(a_input__hasFreshEvent(&downRight->header)) {
            a_input__button_setState(down, downRight->u.leaf.pressed);
            a_input__button_setState(right, downRight->u.leaf.pressed);
        }
    #endif
}
