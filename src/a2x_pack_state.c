/*
    Copyright 2010, 2016 Alex Margarit

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

typedef struct AStateInstance {
    char* name;
    AStateFunction function;
    AStrHash* objects;
    AStateStage stage;
} AStateInstance;

typedef enum {
    A_STATE_ACTION_PUSH,
    A_STATE_ACTION_POP,
} AStateAction;

typedef struct AStatePendingAction {
    AStateAction action;
    char* name;
} AStatePendingAction;

static AStrHash* g_states;
static AList* g_stack;
static AList* g_pending;

static const char* g_stageNames[A_STATE_STAGE_NUM] = {
    "Invalid",
    "Init",
    "Body",
    "Free",
};

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

static AStateInstance* state_new(const char* Name)
{
    AStateFunction function = a_strhash_get(g_states, Name);

    if(function == NULL) {
        a_out__fatal("State '%s' does not exist", Name);
    }

    AStateInstance* const s = a_mem_malloc(sizeof(AStateInstance));

    s->name = a_str_dup(Name);
    s->function = function;
    s->objects = a_strhash_new();
    s->stage = A_STATE_STAGE_INIT;

    a_out__state("New '%s' instance", Name);

    return s;
}

static void state_free(AStateInstance* State)
{
    a_out__stateVerbose("Destroying '%s' instance", State->name);

    free(State->name);
    a_strhash_free(State->objects);

    free(State);
}

static void state_handle(void)
{
    AStateInstance* current = a_list_peek(g_stack);

    // Check if the current state just ran its Free stage
    if(current && current->stage == A_STATE_STAGE_FREE) {
        state_free(current);
        a_system__popCollection();

        a_list_pop(g_stack);
        current = a_list_peek(g_stack);
    }

    // If there are no pending state changes, do any automatic transitions
    if(a_list_empty(g_pending)) {
        if(current && current->stage == A_STATE_STAGE_INIT) {
            current->stage = A_STATE_STAGE_BODY;

            a_out__stateVerbose("  '%s' going from %s to %s",
                                current->name,
                                g_stageNames[A_STATE_STAGE_INIT],
                                g_stageNames[A_STATE_STAGE_BODY]);
        }

        return;
    }

    AStatePendingAction* pending = a_list_pop(g_pending);

    switch(pending->action) {
        case A_STATE_ACTION_PUSH: {
            a_out__stateVerbose("Push '%s'", pending->name);

            A_LIST_ITERATE(g_stack, AStateInstance*, state) {
                if(a_str_equal(pending->name, state->name)) {
                    a_out__fatal("State '%s' already in stack", pending->name);
                }
            }

            AStateInstance* s = state_new(pending->name);
            a_list_push(g_stack, s);
            a_system__pushCollection();
        } break;

        case A_STATE_ACTION_POP: {
            if(current == NULL) {
                a_out__fatal("Pop state: stack is empty");
            }

            a_out__stateVerbose("Pop '%s'", current->name);
            a_out__stateVerbose("  '%s' going from %s to %s",
                                current->name,
                                g_stageNames[current->stage],
                                g_stageNames[A_STATE_STAGE_FREE]);

            current->stage = A_STATE_STAGE_FREE;
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
}

void a_state__uninit(void)
{
    a_strhash_free(g_states);
    a_list_free(g_stack);
    a_list_free(g_pending);
}

void a_state__new(const char* Name, AStateFunction Function)
{
    a_strhash_add(g_states, Name, Function);
    a_out__stateVerbose("Declared '%s'", Name);
}

void a_state_push(const char* Name)
{
    pending_new(A_STATE_ACTION_PUSH, Name);
}

void a_state_pop(void)
{
    pending_new(A_STATE_ACTION_POP, NULL);
}

void a_state_popUntil(const char* Name)
{
    int pops = 0;
    bool found = false;

    A_LIST_ITERATE(g_stack, AStateInstance*, state) {
        if(a_str_equal(state->name, Name)) {
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
    pending_new(A_STATE_ACTION_POP, NULL);
    pending_new(A_STATE_ACTION_PUSH, Name);
}

void a_state_exit(void)
{
    a_out__state("Telling all states to exit");

    for(size_t i = a_list_size(g_stack); i--; ) {
        pending_new(A_STATE_ACTION_POP, NULL);
    }
}

void a_state_add(const char* Name, void* Object)
{
    const AStateInstance* s = a_list_peek(g_stack);

    if(s) {
        a_strhash_add(s->objects, Name, Object);
    }
}

void* a_state_get(const char* Name)
{
    const AStateInstance* s = a_list_peek(g_stack);

    if(s) {
        return a_strhash_get(s->objects, Name);
    }

    return NULL;
}

bool a_state__stage(AStateStage Stage)
{
    const AStateInstance* current = a_list_peek(g_stack);

    if(current != NULL) {
        return current->stage == Stage;
    }

    return false;
}

bool a_state__nothingPending(void)
{
    static bool first = true;

    if(first) {
        first = false;
        a_fps__reset(0);
    } else {
        a_system_run();
        a_fps_frame();
    }

    if(a_list_empty(g_pending)) {
        return true;
    } else {
        first = true;
        return false;
    }
}

void a_state__run(void)
{
    state_handle();

    while(!a_list_empty(g_stack)) {
        AStateInstance* s = a_list_peek(g_stack);

        a_out__stateVerbose("  '%s' running %s",
                            s->name,
                            g_stageNames[s->stage]);

        s->function();
        state_handle();
    }
}
