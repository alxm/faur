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

#if A_PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
#endif

#include "a2x_pack_state.v.h"

typedef struct AState {
    char* name;
    AStateFunction function;
    AStateStage stage;
    AList* tickSystems;
    AList* drawSystems;
} AState;

typedef enum {
    A_STATE_ACTION_PUSH,
    A_STATE_ACTION_POP,
} AStateAction;

typedef struct AStatePendingAction {
    AStateAction action;
    char* name;
} AStatePendingAction;

static AStrHash* g_states; // table of AState
static AList* g_stack; // list of AState
static AList* g_pending; // list of AStatePendingAction
static bool g_exiting;

static const char* stageName(AStateStage Stage)
{
    switch(Stage) {
        case A_STATE__STAGE_INIT:
            return "Init";

        case A_STATE__STAGE_LOOP:
            return "Loop";

        case A_STATE__STAGE_FREE:
            return "Free";

        default:
            return "Invalid";
    }
}

static void pending_new(AStateAction Action, const char* Name)
{
    AStatePendingAction* a = a_mem_malloc(sizeof(AStatePendingAction));

    a->action = Action;
    a->name = Name == NULL ? NULL : a_str_dup(Name);

    a_list_addLast(g_pending, a);
}

static void pending_free(AStatePendingAction* Pending)
{
    if(Pending->name) {
        free(Pending->name);
    }

    free(Pending);
}

static void pending_handle(void)
{
    AState* current = a_list_peek(g_stack);

    // Check if the current state just ran its Free stage
    if(current && current->stage == A_STATE__STAGE_FREE) {
        a_out__stateVerbose("Destroying '%s' instance", current->name);

        a_system__popCollection();
        a_list_pop(g_stack);
        current = a_list_peek(g_stack);
        a_fps__reset(0);
    }

    // If there are no pending state changes, do any automatic transitions
    if(a_list_isEmpty(g_pending)) {
        if(current && current->stage == A_STATE__STAGE_INIT) {
            current->stage = A_STATE__STAGE_LOOP;
            a_fps__reset(0);

            a_out__stateVerbose("  '%s' going from %s to %s",
                                current->name,
                                stageName(A_STATE__STAGE_INIT),
                                stageName(A_STATE__STAGE_LOOP));
        }

        return;
    }

    AStatePendingAction* pending = a_list_pop(g_pending);

    switch(pending->action) {
        case A_STATE_ACTION_PUSH: {
            a_out__stateVerbose("Push '%s'", pending->name);
            AState* state = a_strhash_get(g_states, pending->name);

            if(state == NULL) {
                a_out__fatal("State '%s' does not exist", pending->name);
            }

            A_LIST_ITERATE(g_stack, AState*, s) {
                if(state == s) {
                    a_out__fatal("State '%s' already in stack", pending->name);
                }
            }

            a_out__state("New '%s' instance", pending->name);

            state->stage = A_STATE__STAGE_INIT;
            a_system__pushCollection(state->tickSystems, state->drawSystems);
            a_list_push(g_stack, state);
        } break;

        case A_STATE_ACTION_POP: {
            if(current == NULL) {
                a_out__fatal("Pop state: stack is empty");
            }

            a_out__stateVerbose("Pop '%s'", current->name);
            a_out__stateVerbose("  '%s' going from %s to %s",
                                current->name,
                                stageName(current->stage),
                                stageName(A_STATE__STAGE_FREE));

            current->stage = A_STATE__STAGE_FREE;
        } break;

        default: {
            a_out__fatal("Invalid state action");
        }
    }

    pending_free(pending);
}

void a_state__init(void)
{
    g_states = a_strhash_new();
    g_stack = a_list_new();
    g_pending = a_list_new();
    g_exiting = false;
}

void a_state__uninit(void)
{
    A_STRHASH_ITERATE(g_states, AState*, s) {
        a_list_free(s->tickSystems);
        a_list_free(s->drawSystems);
        free(s->name);
        free(s);
    }

    a_strhash_free(g_states);
    a_list_free(g_stack);
    a_list_free(g_pending);
}

void a_state__new(const char* Name, AStateFunction Function, const char* TickSystems, const char* DrawSystems)
{
    AState* state = a_mem_malloc(sizeof(AState));

    state->name = a_str_dup(Name);
    state->function = Function;
    state->stage = A_STATE__STAGE_INIT;
    state->tickSystems = a_system__parse(TickSystems);
    state->drawSystems = a_system__parse(DrawSystems);

    a_strhash_add(g_states, Name, state);
    a_out__stateVerbose("Declared '%s'", Name);
}

void a_state_push(const char* Name)
{
    a_out__stateVerbose("a_state_push('%s')", Name);

    if(g_exiting) {
        a_out__stateVerbose("  Already exiting, ignoring");
        return;
    }

    pending_new(A_STATE_ACTION_PUSH, Name);
}

void a_state_pop(void)
{
    a_out__stateVerbose("a_state_pop()");

    if(g_exiting) {
        a_out__stateVerbose("  Already exiting, ignoring");
        return;
    }

    pending_new(A_STATE_ACTION_POP, NULL);
}

void a_state_popUntil(const char* Name)
{
    a_out__stateVerbose("a_state_popUntil('%s')", Name);

    if(g_exiting) {
        a_out__stateVerbose("  Already exiting, ignoring");
        return;
    }

    int pops = 0;
    bool found = false;

    A_LIST_ITERATE(g_stack, AState*, s) {
        if(a_str_equal(s->name, Name)) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        a_out__fatal("State '%s' not in stack", Name);
    }

    while(pops--) {
        pending_new(A_STATE_ACTION_POP, NULL);
    }
}

void a_state_replace(const char* Name)
{
    a_out__stateVerbose("a_state_replace('%s')", Name);

    if(g_exiting) {
        a_out__stateVerbose("  Already exiting, ignoring");
        return;
    }

    pending_new(A_STATE_ACTION_POP, NULL);
    pending_new(A_STATE_ACTION_PUSH, Name);
}

void a_state_exit(void)
{
    a_out__state("*** Telling all states to exit ***");

    if(g_exiting) {
        a_out__stateVerbose("  Already exiting, ignoring");
        return;
    }

    g_exiting = true;

    // Clear the pending actions queue
    a_list_clearEx(g_pending, (AFree*)pending_free);

    // Queue a pop for every state in the stack
    for(unsigned i = a_list_getSize(g_stack); i--; ) {
        pending_new(A_STATE_ACTION_POP, NULL);
    }
}

static bool iteration(void)
{
    pending_handle();

    AState* s = a_list_peek(g_stack);

    if(s == NULL) {
        return false;
    }

    if(s->stage == A_STATE__STAGE_LOOP) {
        while(a_fps__tick() && a_list_isEmpty(g_pending)) {
            a_input__get();
            s->function(A_STATE__STAGE_LOOP | A_STATE__STAGE_TICK);
            a_system__tick();
        }

        s->function(A_STATE__STAGE_LOOP | A_STATE__STAGE_DRAW);
        a_system__draw();
        a_screen__show();

        a_fps__frame();
    } else {
        a_out__stateVerbose("  '%s' running %s", s->name, stageName(s->stage));
        s->function(s->stage);
    }

    return true;
}

#if A_PLATFORM_EMSCRIPTEN
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

    #if A_PLATFORM_EMSCRIPTEN
        emscripten_set_main_loop(loop,
                                 a_settings_getBool("video.vsync")
                                     ? 0
                                     : (int)a_settings_getUnsigned("video.fps"),
                                 true);
    #else
        while(iteration()) {
            continue;
        }

        a_out__message("Finished running states");
    #endif
}
