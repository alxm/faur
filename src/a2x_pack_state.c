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

#include "a2x_pack_state.v.h"

#if A_BUILD_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_console.v.h"
#include "a2x_pack_ecs.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_timer.v.h"

struct AState {
    char* name;
    AStateFunction* function;
    AStateStage stage;
};

static AList* g_states; // list of AState
static AList* g_stack; // list of AState
static AList* g_pending; // list of AState
static bool g_exiting;

static const char* g_stageNames[A__STATE_STAGE_NUM] = {
    [A__STATE_STAGE_INIT] = "Init",
    [A__STATE_STAGE_LOOP] = "Loop",
    [A__STATE_STAGE_FREE] = "Free",
};

static void pending_push(AState* State)
{
    a_list_addLast(g_pending, State);
}

static void pending_pop(void)
{
    a_list_addLast(g_pending, NULL);
}

static void pending_handle(void)
{
    AState* current = a_list_peek(g_stack);

    // Check if the current state just ran its Free stage
    if(current && current->stage == A__STATE_STAGE_FREE) {
        a_out__statev("Destroying '%s' instance", current->name);

        a_ecs__collectionPop();
        a_list_pop(g_stack);

        current = a_list_peek(g_stack);

        if(a_list_isEmpty(g_pending)
            && current && current->stage == A__STATE_STAGE_LOOP) {

            a_fps__reset();
        }
    }

    // If there are no pending state changes,
    // check if the current state should transition from Init to Loop
    if(a_list_isEmpty(g_pending)) {
        if(current && current->stage == A__STATE_STAGE_INIT) {
            a_out__statev("  '%s' going from %s to %s",
                          current->name,
                          g_stageNames[A__STATE_STAGE_INIT],
                          g_stageNames[A__STATE_STAGE_LOOP]);

            current->stage = A__STATE_STAGE_LOOP;

            a_fps__reset();
        }

        return;
    }

    AState* pendingState = a_list_pop(g_pending);

    if(pendingState == NULL) {
        if(current == NULL) {
            a_out__fatal("Pop state: stack is empty");
        }

        a_out__statev("Pop '%s'", current->name);
        a_out__statev("  '%s' going from %s to %s",
                      current->name,
                      g_stageNames[current->stage],
                      g_stageNames[A__STATE_STAGE_FREE]);

        current->stage = A__STATE_STAGE_FREE;
    } else {
        a_out__statev("Push '%s'", pendingState->name);

        if(pendingState == NULL) {
            a_out__fatal("State '%s' does not exist", pendingState->name);
        }

        A_LIST_ITERATE(g_stack, AState*, s) {
            if(pendingState == s) {
                a_out__fatal("State '%s' already in stack", pendingState->name);
            }
        }

        a_out__state("New '%s' instance", pendingState->name);

        pendingState->stage = A__STATE_STAGE_INIT;

        a_list_push(g_stack, pendingState);
        a_ecs__collectionPush();
    }
}

AState* a_state_new(const char* Name, AStateFunction* Function)
{
    AState* state = a_mem_malloc(sizeof(AState));

    state->name = a_str_dup(Name);
    state->function = Function;
    state->stage = A__STATE_STAGE_INIT;

    a_list_addLast(g_states, state);

    return state;
}

static void a_state__free(AState* State)
{
    free(State->name);
    free(State);
}

void a_state__init(void)
{
    g_states = a_list_new();
    g_stack = a_list_new();
    g_pending = a_list_new();
    g_exiting = false;
}

void a_state__uninit(void)
{
    a_list_freeEx(g_states, (AFree*)a_state__free);
    a_list_free(g_stack);
    a_list_free(g_pending);
}

void a_state_push(AState* State)
{
    a_out__statev("a_state_push('%s')", State->name);

    if(g_exiting) {
        a_out__statev("a_state_push: Already exiting");
        return;
    }

    pending_push(State);
}

void a_state_pop(void)
{
    a_out__statev("a_state_pop()");

    if(g_exiting) {
        a_out__statev("a_state_pop: Already exiting");
        return;
    }

    pending_pop();
}

void a_state_popUntil(AState* State)
{
    a_out__statev("a_state_popUntil('%s')", State->name);

    if(g_exiting) {
        a_out__statev("a_state_popUntil: Already exiting");
        return;
    }

    int pops = 0;
    bool found = false;

    A_LIST_ITERATE(g_stack, AState*, s) {
        if(State == s) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        a_out__fatal("a_state_popUntil: State '%s' not in stack", State->name);
    }

    while(pops--) {
        pending_pop();
    }
}

void a_state_replace(AState* State)
{
    a_out__statev("a_state_replace('%s')", State->name);

    if(g_exiting) {
        a_out__statev("a_state_replace: Already exiting");
        return;
    }

    pending_pop();
    pending_push(State);
}

void a_state_exit(void)
{
    if(g_exiting) {
        a_out__statev("a_state_exit: Already exiting");
        return;
    }

    a_out__state("*** Telling all states to exit ***");

    g_exiting = true;

    // Clear the pending actions queue
    a_list_clear(g_pending);

    // Queue a pop for every state in the stack
    for(unsigned i = a_list_sizeGet(g_stack); i--; ) {
        pending_pop();
    }
}

static bool iteration(void)
{
    pending_handle();

    AState* s = a_list_peek(g_stack);

    if(s == NULL) {
        return false;
    }

    if(s->stage == A__STATE_STAGE_LOOP) {
        while(a_fps__tick()) {
            a_timer__tick();
            a_input__tick();
            a_sound__tick();
            a_screen__tick();
            a_screenshot__tick();
            a_console__tick();
            s->function(A__STATE_STAGE_LOOP, true);
            a_ecs__tick();

            if(!a_list_isEmpty(g_pending)) {
                return true;
            }
        }

        s->function(A__STATE_STAGE_LOOP, false);
        a_ecs__draw();
        a_sound__draw();
        a_console__draw();
        a_screen__draw();

        a_fps__frame();
    } else {
        a_out__statev("  '%s' running %s", s->name, g_stageNames[s->stage]);
        s->function(s->stage, false);
    }

    return true;
}

#if A_BUILD_SYSTEM_EMSCRIPTEN
static void loop(void)
{
    if(!iteration()) {
        a_out__message("Finished running states");
        emscripten_cancel_main_loop();
    }
}
#endif

void a_state__run(void)
{
    a_out__message("Running states");

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        emscripten_set_main_loop(loop,
                                 a_settings_boolGet(A_SETTING_VIDEO_VSYNC)
                                     ? 0
                                     : (int)
                                        a_settings_intuGet(A_SETTING_FPS_DRAW),
                                 true);
    #else
        while(iteration()) {
            continue;
        }

        a_out__message("Finished running states");
    #endif
}
