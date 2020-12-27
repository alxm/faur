/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#define FAUR_IMPLEMENT_STATE

#include "f_state.v.h"
#include <faur.v.h>

static FListIntr g_stack = F_LISTINTR_NEW(g_stack, FStateEntry, listNode);
static FListIntr g_pending = F_LISTINTR_NEW(g_pending, FStateEntry, listNode);

static bool g_exiting;
static const FEvent* g_blockEvent;

static FCallState *g_tickPre, *g_tickPost, *g_drawPre, *g_drawPost;

#if F_CONFIG_DEBUG
static const char* g_stageNames[F__STATE_STAGE_NUM] = {
    [F__STATE_STAGE_INIT] = "Init",
    [F__STATE_STAGE_TICK] = "Loop",
    [F__STATE_STAGE_DRAW] = "Loop",
    [F__STATE_STAGE_FREE] = "Free",
};
#endif

static void pending_push(FCallState* Handler, const char* Name)
{
    FStateEntry* e = f_pool__alloc(F_POOL__STACK_STATE);

    e->name = Name;
    e->handler = Handler;
    e->stage = F__STATE_STAGE_INVALID;

    f_listintr_addLast(&g_pending, e);
}

static void pending_pop(void)
{
    f_listintr_addLast(&g_pending, f_pool__alloc(F_POOL__STACK_STATE));
}

static void pending_handle(void)
{
    FStateEntry* current = f_listintr_peek(&g_stack);

    if(current) {
        bool resetFps = false;

        if(current->stage == F__STATE_STAGE_INIT) {
            #if F_CONFIG_DEBUG
                f_out__state("'%s' going from %s to %s",
                             current->name,
                             g_stageNames[F__STATE_STAGE_INIT],
                             g_stageNames[F__STATE_STAGE_TICK]);
            #endif

            current->stage = F__STATE_STAGE_TICK;
            resetFps = true;
        } else if(current->stage == F__STATE_STAGE_FREE) {
            #if F_CONFIG_DEBUG
                f_out__state("Destroying '%s' instance", current->name);
            #endif

            f_pool_release(f_listintr_pop(&g_stack));

            current = f_listintr_peek(&g_stack);
            resetFps = current && current->stage == F__STATE_STAGE_TICK;
        }

        if(resetFps && !g_exiting && f_listintr_sizeIsEmpty(&g_pending)) {
            f_fps__reset();
        }
    }

    if(f_listintr_sizeIsEmpty(&g_pending) || f_state_blockGet()) {
        return;
    }

    FStateEntry* pendingState = f_listintr_pop(&g_pending);

    if(pendingState->handler == NULL) {
        #if F_CONFIG_DEBUG
            if(current == NULL) {
                F__FATAL("Pop state: stack is empty");
            }

            f_out__state("Pop '%s'", current->name);

            f_out__state("'%s' going from %s to %s",
                         current->name,
                         g_stageNames[current->stage],
                         g_stageNames[F__STATE_STAGE_FREE]);
        #endif

        current->stage = F__STATE_STAGE_FREE;

        f_pool_release(pendingState);
    } else {
        #if F_CONFIG_DEBUG
            f_out__state("Push '%s'", pendingState->name);

            F_LISTINTR_ITERATE(&g_stack, const FStateEntry*, e) {
                if(pendingState->handler == e->handler) {
                    F__FATAL("State '%s' already in stack", e->name);
                }
            }
        #endif

        f_out__state("New '%s' instance", pendingState->name);

        pendingState->stage = F__STATE_STAGE_INIT;

        f_listintr_push(&g_stack, pendingState);
    }
}

static void f_state__uninit(void)
{
    f_listintr_apply(&g_stack, f_pool_release);
    f_listintr_apply(&g_pending, f_pool_release);
}

const FPack f_pack__state = {
    "State",
    NULL,
    f_state__uninit,
};

void f_state_callbacks(FCallState* TickPre, FCallState* TickPost, FCallState* DrawPre, FCallState* DrawPost)
{
    g_tickPre = TickPre;
    g_tickPost = TickPost;
    g_drawPre = DrawPre;
    g_drawPost = DrawPost;
}

void f_state_push(FCallState* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_DEBUG
            f_out__state("f_state_push(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_DEBUG
        f_out__state("f_state_push(%s)", Name);
    #endif

    pending_push(Handler, Name);
}

void f_state_pop(void)
{
    if(g_exiting) {
        #if F_CONFIG_DEBUG
            f_out__state("f_state_pop: Already exiting");
        #endif

        return;
    }

    #if F_CONFIG_DEBUG
        f_out__state("f_state_pop()");
    #endif

    pending_pop();
}

void f_state_popUntil(FCallState* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_DEBUG
            f_out__state("f_state_popUntil(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_DEBUG
        f_out__state("f_state_popUntil(%s)", Name);
    #endif

    int pops = 0;
    bool found = false;

    F_LISTINTR_ITERATE(&g_stack, const FStateEntry*, e) {
        if(e->handler == Handler) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        F__FATAL("f_state_popUntil(%s): State not in stack", Name);
    }

    while(pops--) {
        pending_pop();
    }
}

void f_state_replace(FCallState* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_DEBUG
            f_out__state("f_state_replace(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_DEBUG
        f_out__state("f_state_replace(%s)", Name);
    #endif

    pending_pop();
    pending_push(Handler, Name);
}

void f_state_exit(void)
{
    if(g_exiting) {
        #if F_CONFIG_DEBUG
            f_out__state("f_state_exit: Already exiting");
        #endif

        return;
    }

    f_out__state("*** Telling all states to exit ***");

    g_exiting = true;

    // Clear the pending actions queue
    f_listintr_apply(&g_pending, f_pool_release);

    // Queue a pop for every state in the stack
    F_LISTINTR_ITERATE(&g_stack, const FStateEntry*, e) {
        pending_pop();
    }
}

FCallState* f_state_currentGet(void)
{
    FStateEntry* current = f_listintr_peek(&g_stack);

    if(current == NULL) {
        F__FATAL("f_state_currentGet: Stack is empty");
    }

    return current->handler;
}

bool f_state_currentChanged(void)
{
    return !f_listintr_sizeIsEmpty(&g_pending);
}

bool f_state_blockGet(void)
{
    if(g_blockEvent) {
        if(*g_blockEvent != 0) {
            return true;
        }

        g_blockEvent = NULL;
    }

    return false;
}

void f_state_blockSet(const FEvent* Event)
{
    g_blockEvent = Event;
}

bool f_state__runStep(void)
{
    pending_handle();

    FStateEntry* s = f_listintr_peek(&g_stack);

    if(s == NULL) {
        return false;
    }

    if(s->stage == F__STATE_STAGE_TICK) {
        while(f_fps__tick()) {
            f_timer__tick();
            f_input__tick();
            f_sound__tick();
            f_screen__tick();
            f_screenshot__tick();
            f_console__tick();
            f_entity__tick();
            f_fade__tick();

            if(!f_listintr_sizeIsEmpty(&g_pending) && !f_state_blockGet()) {
                return true;
            }

            if(g_tickPre) {
                g_tickPre();
            }

            s->handler();

            if(g_tickPost) {
                g_tickPost();
            }

            if(!f_listintr_sizeIsEmpty(&g_pending) && !f_state_blockGet()) {
                return true;
            }
        }

        s->stage = F__STATE_STAGE_DRAW;

        if(g_drawPre) {
            g_drawPre();
        }

        s->handler();

        if(g_drawPost) {
            g_drawPost();
        }

        s->stage = F__STATE_STAGE_TICK;

        f_align_reset();
        f_color_reset();
        f_font_reset();
        f_screen_clipReset();

        f_fade__draw();
        f_sound__draw();
        f_console__draw();
        f_screen__draw();

        f_fps__frame();
    } else {
        #if F_CONFIG_DEBUG
            f_out__state("'%s' running %s", s->name, g_stageNames[s->stage]);
        #endif

        s->handler();
    }

    return true;
}

bool f__state_stageCheck(F__StateStage Stage)
{
    const FStateEntry* e = f_listintr_peek(&g_stack);

    #if F_CONFIG_DEBUG
        if(e == NULL) {
            F__FATAL("%s: state stack is empty", g_stageNames[Stage]);
        }
    #endif

    return e->stage == Stage;
}
