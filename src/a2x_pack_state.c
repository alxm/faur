/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_state.v.h"

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_console.v.h"
#include "a2x_pack_ecs.v.h"
#include "a2x_pack_fade.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_timer.v.h"

typedef struct {
    AStateHandler* function;
    const char* name;
} AStateTableEntry;

typedef struct {
    const AStateTableEntry* state;
    AStateStage stage;
} AStateStackEntry;

static AList* g_stack; // list of AStateStackEntry
static AList* g_pending; // list of AStateStackEntry/NULL
static bool g_exiting;
static const AEvent* g_blockEvent;
static AStateTableEntry g_table[A_CONFIG_STATE_NUM];

static const char* g_stageNames[A__STATE_STAGE_NUM] = {
    [A__STATE_STAGE_INIT] = "Init",
    [A__STATE_STAGE_TICK] = "Loop",
    [A__STATE_STAGE_DRAW] = "Loop",
    [A__STATE_STAGE_FREE] = "Free",
};

static void pending_push(const AStateTableEntry* State)
{
    AStateStackEntry* e = a_mem_malloc(sizeof(AStateStackEntry));

    e->state = State;
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
        a_out__stateV("Destroying '%s' instance", current->state->name);

        free(a_list_pop(g_stack));
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
            a_out__stateV("  '%s' going from %s to %s",
                          current->state->name,
                          g_stageNames[A__STATE_STAGE_INIT],
                          g_stageNames[A__STATE_STAGE_TICK]);

            current->stage = A__STATE_STAGE_TICK;

            a_fps__reset();
        }

        return;
    }

    AStateStackEntry* pendingState = a_list_pop(g_pending);

    if(pendingState == NULL) {
        if(current == NULL) {
            A__FATAL("Pop state: stack is empty");
        }

        a_out__stateV("Pop '%s'", current->state->name);
        a_out__stateV("  '%s' going from %s to %s",
                      current->state->name,
                      g_stageNames[current->stage],
                      g_stageNames[A__STATE_STAGE_FREE]);

        current->stage = A__STATE_STAGE_FREE;
    } else {
        a_out__stateV("Push '%s'", pendingState->state->name);

        A_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
            if(pendingState->state == e->state) {
                A__FATAL("State '%s' already in stack", e->state->name);
            }
        }

        a_out__state("New '%s' instance", pendingState->state->name);
        a_list_push(g_stack, pendingState);
    }
}

void a_state__init(void)
{
    for(int i = A_CONFIG_STATE_NUM; i--; ) {
        g_table[i].name = "";
    }

    g_stack = a_list_new();
    g_pending = a_list_new();
}

void a_state__uninit(void)
{
    a_list_freeEx(g_stack, free);
    a_list_freeEx(g_pending, free);
}

void a_state_new(int Index, AStateHandler* Handler, const char* Name)
{
    g_table[Index].function = Handler;
    g_table[Index].name = Name;
}

static const AStateTableEntry* getState(int State, const char* CallerFunction)
{
    #if A_CONFIG_BUILD_DEBUG
        if(State < 0 || State >= A_CONFIG_STATE_NUM) {
            A__FATAL("%s(%d): Unknown state", CallerFunction, State);
        }

        if(g_table[State].function == NULL) {
            A__FATAL("%s(%d): Uninitialized state", CallerFunction, State);
        }
    #else
        A_UNUSED(CallerFunction);
    #endif

    return &g_table[State];
}

void a_state_push(int State)
{
    const AStateTableEntry* state = getState(State, __func__);

    if(g_exiting) {
        a_out__stateV("a_state_push(%s): Already exiting", state->name);
        return;
    } else {
        a_out__stateV("a_state_push(%s)", state->name);
    }

    pending_push(state);
}

void a_state_pop(void)
{
    if(g_exiting) {
        a_out__stateV("a_state_pop: Already exiting");
        return;
    } else {
        a_out__stateV("a_state_pop()");
    }

    pending_pop();
}

void a_state_popUntil(int State)
{
    const AStateTableEntry* state = getState(State, __func__);

    if(g_exiting) {
        a_out__stateV("a_state_popUntil(%s): Already exiting", state->name);
        return;
    } else {
        a_out__stateV("a_state_popUntil(%s)", state->name);
    }

    int pops = 0;
    bool found = false;

    A_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
        if(e->state == state) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        A__FATAL("a_state_popUntil(%s): State not in stack", state->name);
    }

    while(pops--) {
        pending_pop();
    }
}

void a_state_replace(int State)
{
    const AStateTableEntry* state = getState(State, __func__);

    if(g_exiting) {
        a_out__stateV("a_state_replace(%s): Already exiting", state->name);
        return;
    } else {
        a_out__stateV("a_state_replace(%s)", state->name);
    }

    pending_pop();
    pending_push(state);
}

void a_state_exit(void)
{
    if(g_exiting) {
        a_out__stateV("a_state_exit: Already exiting");
        return;
    }

    a_out__state("*** Telling all states to exit ***");

    g_exiting = true;

    // Clear the pending actions queue
    a_list_clearEx(g_pending, free);

    // Queue a pop for every state in the stack
    for(unsigned i = a_list_sizeGet(g_stack); i--; ) {
        pending_pop();
    }
}

bool a_state_blockGet(void)
{
    return g_blockEvent && *g_blockEvent != 0;
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
        g_blockEvent = NULL;
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
                g_blockEvent = NULL;
                return true;
            }

            s->state->function();

            if(!a_list_isEmpty(g_pending) && !a_state_blockGet()) {
                g_blockEvent = NULL;
                return true;
            }
        }

        s->stage = A__STATE_STAGE_DRAW;
        s->state->function();
        s->stage = A__STATE_STAGE_TICK;

        a_fade__draw();
        a_sound__draw();
        a_console__draw();
        a_screen__draw();

        a_fps__frame();
    } else {
        a_out__stateV(
            "  '%s' running %s", s->state->name, g_stageNames[s->stage]);
        s->state->function();
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

    if(e == NULL) {
        A__FATAL("%s: state stack is empty", g_stageNames[Stage]);
    }

    return e->stage == Stage;
}
