/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#define A2X_IMPLEMENT 1

#include "a_state.v.h"
#include <a2x.v.h>

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

typedef struct {
    const char* name;
    AStateHandler* handler;
    AStateStage stage;
} AStateStackEntry;

static AList* g_stack; // list of AStateStackEntry
static AList* g_pending; // list of AStateStackEntry/NULL
static bool g_exiting;
static const AEvent* g_blockEvent;

#if A_CONFIG_BUILD_DEBUG
static const char* g_stageNames[A__STATE_STAGE_NUM] = {
    [A__STATE_STAGE_INIT] = "Init",
    [A__STATE_STAGE_TICK] = "Loop",
    [A__STATE_STAGE_DRAW] = "Loop",
    [A__STATE_STAGE_FREE] = "Free",
};
#endif

static void pending_push(AStateHandler* Handler, const char* Name)
{
    AStateStackEntry* e = a_mem_malloc(sizeof(AStateStackEntry));

    e->name = Name;
    e->handler = Handler;
    e->stage = A__STATE_STAGE_INIT;

    a_list_addLast(g_pending, e);
}

static void pending_pop(void)
{
    a_list_addLast(g_pending, NULL);
}

static void pending_handle(void)
{
    AStateStackEntry* current = a_list_peek(g_stack);

    // Check if the current state just ran its Free stage
    if(current && current->stage == A__STATE_STAGE_FREE) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("Destroying '%s' instance", current->name);
        #endif

        a_mem_free(a_list_pop(g_stack));
        current = a_list_peek(g_stack);

        if(!g_exiting && a_list_isEmpty(g_pending)
            && current && current->stage == A__STATE_STAGE_TICK) {

            a_fps__reset();
        }
    }

    // If there are no pending state changes,
    // check if the current state should transition from Init to Loop
    if(a_list_isEmpty(g_pending)) {
        if(current && current->stage == A__STATE_STAGE_INIT) {
            #if A_CONFIG_BUILD_DEBUG
                a_out__state("'%s' going from %s to %s",
                             current->name,
                             g_stageNames[A__STATE_STAGE_INIT],
                             g_stageNames[A__STATE_STAGE_TICK]);
            #endif

            current->stage = A__STATE_STAGE_TICK;

            a_fps__reset();
        }

        return;
    }

    AStateStackEntry* pendingState = a_list_pop(g_pending);

    if(pendingState == NULL) {
        #if A_CONFIG_BUILD_DEBUG
            if(current == NULL) {
                A__FATAL("Pop state: stack is empty");
            }

            a_out__state("Pop '%s'", current->name);

            a_out__state("'%s' going from %s to %s",
                         current->name,
                         g_stageNames[current->stage],
                         g_stageNames[A__STATE_STAGE_FREE]);
        #endif

        current->stage = A__STATE_STAGE_FREE;
    } else {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("Push '%s'", pendingState->name);

            A_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
                if(pendingState->handler == e->handler) {
                    A__FATAL("State '%s' already in stack", e->name);
                }
            }
        #endif

        a_out__state("New '%s' instance", pendingState->name);
        a_list_push(g_stack, pendingState);
    }
}

static void a_state__init(void)
{
    g_stack = a_list_new();
    g_pending = a_list_new();
}

static void a_state__uninit(void)
{
    a_list_freeEx(g_stack, a_mem_free);
    a_list_freeEx(g_pending, a_mem_free);
}

const APack a_pack__state = {
    "State",
    {
        [0] = a_state__init,
    },
    {
        [0] = a_state__uninit,
    },
};

void a_state_push(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("a_state_push(%s): Already exiting", Name);
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        a_out__state("a_state_push(%s)", Name);
    #endif

    pending_push(Handler, Name);
}

void a_state_pop(void)
{
    if(g_exiting) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("a_state_pop: Already exiting");
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        a_out__state("a_state_pop()");
    #endif

    pending_pop();
}

void a_state_popUntil(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("a_state_popUntil(%s): Already exiting", Name);
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        a_out__state("a_state_popUntil(%s)", Name);
    #endif

    int pops = 0;
    bool found = false;

    A_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
        if(e->handler == Handler) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        A__FATAL("a_state_popUntil(%s): State not in stack", Name);
    }

    while(pops--) {
        pending_pop();
    }
}

void a_state_replace(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("a_state_replace(%s): Already exiting", Name);
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        a_out__state("a_state_replace(%s)", Name);
    #endif

    pending_pop();
    pending_push(Handler, Name);
}

void a_state_exit(void)
{
    if(g_exiting) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("a_state_exit: Already exiting");
        #endif

        return;
    }

    a_out__state("*** Telling all states to exit ***");

    g_exiting = true;

    // Clear the pending actions queue
    a_list_clearEx(g_pending, a_mem_free);

    // Queue a pop for every state in the stack
    for(unsigned i = a_list_sizeGet(g_stack); i--; ) {
        pending_pop();
    }
}

AStateHandler* a_state_currentGet(void)
{
    AStateStackEntry* current = a_list_peek(g_stack);

    if(current == NULL) {
        A__FATAL("a_state_currentGet: Stack is empty");
    }

    return current->handler;
}

bool a_state_currentChanged(void)
{
    return !a_list_isEmpty(g_pending);
}

bool a_state_blockGet(void)
{
    if(g_blockEvent) {
        if(*g_blockEvent != 0) {
            return true;
        }

        g_blockEvent = NULL;
    }

    return false;
}

void a_state_blockSet(const AEvent* Event)
{
    g_blockEvent = Event;
}

static bool iteration(void)
{
    #if A_CONFIG_SYSTEM_EMSCRIPTEN
        if(!EM_ASM_INT({ return Module.a2x_fsIsReady; }, 0)) {
            return true;
        }
    #endif

    if(!a_state_blockGet()) {
        pending_handle();
    }

    AStateStackEntry* s = a_list_peek(g_stack);

    if(s == NULL) {
        return false;
    }

    if(s->stage == A__STATE_STAGE_TICK) {
        while(a_fps__tick()) {
            a_timer__tick();
            a_input__tick();
            a_sound__tick();
            a_screen__tick();
            a_screenshot__tick();
            a_console__tick();
            a_ecs__tick();
            a_fade__tick();

            if(!a_list_isEmpty(g_pending) && !a_state_blockGet()) {
                return true;
            }

            s->handler();

            if(!a_list_isEmpty(g_pending) && !a_state_blockGet()) {
                return true;
            }
        }

        s->stage = A__STATE_STAGE_DRAW;
        s->handler();
        s->stage = A__STATE_STAGE_TICK;

        a_fade__draw();
        a_sound__draw();
        a_console__draw();
        a_screen__draw();

        a_fps__frame();
    } else {
        #if A_CONFIG_BUILD_DEBUG
            a_out__state("'%s' running %s", s->name, g_stageNames[s->stage]);
        #endif

        s->handler();
    }

    return true;
}

#if A_CONFIG_SYSTEM_EMSCRIPTEN
static void loop(void)
{
    if(!iteration()) {
        a_out__state("Finished running states");
        emscripten_cancel_main_loop();
    }
}
#endif

void a_state__run(void)
{
    a_out__state("Running states");

    #if A_CONFIG_SYSTEM_EMSCRIPTEN
        emscripten_set_main_loop(
            loop,
            a_platform_api__screenVsyncGet() ? 0 : A_CONFIG_FPS_RATE_DRAW,
            true);
    #else
        while(iteration()) {
            continue;
        }

        a_out__state("Finished running states");
    #endif
}

bool a__state_stageCheck(AStateStage Stage)
{
    const AStateStackEntry* e = a_list_peek(g_stack);

    #if A_CONFIG_BUILD_DEBUG
        if(e == NULL) {
            A__FATAL("%s: state stack is empty", g_stageNames[Stage]);
        }
    #endif

    return e->stage == Stage;
}
