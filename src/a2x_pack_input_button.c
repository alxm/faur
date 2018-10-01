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
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strbuilder.v.h"
#include "a2x_pack_timer.v.h"

struct AButton {
    AInputUserHeader header;
    AListNode* listNode;
    AList* combos; // List of lists of APlatformButton, each a buttons combo
    ATimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

static AList* g_buttons; // list of AButton

void a_input_button__init(void)
{
    g_buttons = a_list_new();
}

void a_input_button__uninit(void)
{
    a_list_free(g_buttons);
}

AButton* a_button_new(const char* Ids)
{
    AButton* b = a_mem_malloc(sizeof(AButton));

    a_input__userHeaderInit(&b->header);

    b->listNode = a_list_addLast(g_buttons, b);
    b->combos = a_list_new();
    b->autoRepeat = NULL;
    b->isClone = false;
    b->waitForRelease = false;
    b->pressed = false;

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        if(a_str_indexGetFirst(id, '+') > 0) {
            AList* combo = a_list_new();
            AList* tok = a_str_split(id, "+");
            bool missing = false;

            A_LIST_ITERATE(tok, char*, part) {
                APlatformButton* pb = a_platform__buttonGet(part);

                if(pb == NULL) {
                    missing = true;
                    break;
                }

                a_list_addLast(combo, pb);
            }

            if(missing || a_list_isEmpty(combo)) {
                a_list_free(combo);
            } else {
                a_list_addLast(b->combos, combo);
            }

            a_list_freeEx(tok, free);
        } else {
            APlatformButton* pb = a_platform__buttonGet(id);

            if(pb) {
                a_list_addLast(b->header.platformInputs, pb);
            }
        }
    }

    a_list_freeEx(tok, free);

    if(!a_list_isEmpty(b->header.platformInputs)) {
        APlatformButton* pb = a_list_getLast(b->header.platformInputs);
        b->header.name = a_str_dup(a_platform__buttonNameGet(pb));
    } else if(!a_list_isEmpty(b->combos)) {
        AStrBuilder* sb = a_strbuilder_new(128);
        AList* combo = a_list_getLast(b->combos);

        A_LIST_ITERATE(combo, APlatformButton*, pb) {
            a_strbuilder_add(sb, a_platform__buttonNameGet(pb));

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

AButton* a_button_dup(const AButton* Button)
{
    AButton* b = a_mem_dup(Button, sizeof(AButton));

    b->listNode = a_list_addLast(g_buttons, b);
    b->autoRepeat = NULL;
    b->isClone = true;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

void a_button_free(AButton* Button)
{
    if(Button == NULL) {
        return;
    }

    a_list_removeNode(Button->listNode);

    if(!Button->isClone) {
        a_list_freeEx(Button->combos, (AFree*)a_list_free);
        a_input__userHeaderFree(&Button->header);
    }

    a_timer_free(Button->autoRepeat);

    free(Button);
}

bool a_button_isWorking(const AButton* Button)
{
    return !a_list_isEmpty(Button->header.platformInputs)
        || !a_list_isEmpty(Button->combos);
}

const char* a_button_nameGet(const AButton* Button)
{
    return Button->header.name;
}

bool a_button_pressGet(const AButton* Button)
{
    return Button->pressed;
}

bool a_button_pressGetOnce(AButton* Button)
{
    bool pressed = a_button_pressGet(Button);

    if(pressed) {
        a_button_pressClear(Button);
    }

    return pressed;
}

void a_button_pressSetRepeat(AButton* Button, unsigned RepeatMs)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = a_timer_new(A_TIMER_MS, RepeatMs, true);
    } else {
        a_timer_stop(Button->autoRepeat);
        a_timer_periodSet(Button->autoRepeat, RepeatMs);
    }
}

void a_button_pressClear(AButton* Button)
{
    Button->waitForRelease = true;
    Button->pressed = false;
}

void a_input_button__tick(void)
{
    A_LIST_ITERATE(g_buttons, AButton*, b) {
        bool pressed = false;

        A_LIST_ITERATE(b->header.platformInputs, APlatformButton*, pb) {
            if(a_platform__buttonPressGet(pb)) {
                pressed = true;
                goto done;
            }
        }

        A_LIST_ITERATE(b->combos, AList*, andList) {
            A_LIST_ITERATE(andList, APlatformButton*, pb) {
                if(!a_platform__buttonPressGet(pb)) {
                    break;
                } else if(A_LIST_IS_LAST()) {
                    pressed = true;
                    goto done;
                }
            }
        }

done:
        if(b->waitForRelease && !pressed) {
            b->waitForRelease = false;
        }

        if(b->autoRepeat) {
            if(pressed) {
                if(!a_timer_isRunning(b->autoRepeat)) {
                    a_timer_start(b->autoRepeat);
                } else if(!a_timer_expiredGet(b->autoRepeat)) {
                    pressed = false;
                }
            } else {
                a_timer_stop(b->autoRepeat);
            }
        }

        b->pressed = pressed && !b->waitForRelease;
    }
}
