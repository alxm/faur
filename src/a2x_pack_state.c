/*
    Copyright 2010 Alex Margarit

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

struct AStateInstance {
    char* name;
    AStateFunction function;
    AStrHash* objects;
    AStateStage stage;
    AStateSubStage substage;
};

typedef enum {
    A_STATE_ACTION_NONE,
    A_STATE_ACTION_PUSH,
    A_STATE_ACTION_POP,
    A_STATE_ACTION_PAUSE,
    A_STATE_ACTION_RESUME,
    A_STATE_ACTION_EXIT,
    A_STATE_ACTION_MAX
} AStateAction;

typedef struct AStatePendingAction {
    AStateAction action;
    char* name;
} AStatePendingAction;

static AStrHash* g_states;
static AList* g_stack;
static AList* g_pending;

static char* g_stageNames[A_STATE_STAGE_NUM] = {
    "Invalid",
    "Init",
    "Body",
    "Free",
};

static char* g_subStageNames[A_STATE_SUBSTAGE_NUM] = {
    "Invalid",
    "Running",
    "Paused",
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
    s->substage = A_STATE_SUBSTAGE_INVALID;

    a_out__state("New '%s' instance", Name);

    return s;
}

static void state_free(AStateInstance* State)
{
    a_out__state("Destroying '%s' instance", State->name);

    free(State->name);
    a_strhash_free(State->objects);

    free(State);
}

static void state_handle(void)
{
    // If there are no pending state changes, do any automatic transitions
    if(a_list_isEmpty(g_pending)) {
        AStateInstance* current = a_list_peek(g_stack);

        if(current && current->stage == A_STATE_STAGE_INIT) {
            current->stage = A_STATE_STAGE_BODY;
            current->substage = A_STATE_SUBSTAGE_RUN;

            a_out__state("'%s' going from %s to %s/%s",
                current->name,
                g_stageNames[A_STATE_STAGE_INIT],
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[A_STATE_SUBSTAGE_RUN]);
        }

        return;
    }

    AStatePendingAction* pending = a_list_pop(g_pending);

    switch(pending->action) {
        case A_STATE_ACTION_PUSH: {
            AStateInstance* s = state_new(pending->name);
            a_list_push(g_stack, s);
        } break;

        case A_STATE_ACTION_POP: {
            AStateInstance* s = a_list_peek(g_stack);

            if(s == NULL) {
                a_out__fatal("Pop state: stack is empty");
            } else if(s->stage != A_STATE_STAGE_BODY) {
                a_out__fatal("Pop state '%s': only call from A_STATE_BODY",
                    s->name);
            }

            a_out__state("'%s' going from %s/%s to %s",
                s->name,
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[s->substage],
                g_stageNames[A_STATE_STAGE_FREE]);

            s->stage = A_STATE_STAGE_FREE;
            s->substage = A_STATE_SUBSTAGE_INVALID;
        } break;

        case A_STATE_ACTION_PAUSE: {
            AStateInstance* s = a_list_peek(g_stack);

            if(s == NULL) {
                a_out__fatal("Pause state: stack is empty");
            } else if(s->stage != A_STATE_STAGE_BODY) {
                a_out__fatal("Pause state '%s': only call from A_STATE_BODY",
                    s->name);
            } else if(s->substage != A_STATE_SUBSTAGE_RUN) {
                a_out__fatal("Pause state '%s': only call from A_STATE_RUN",
                    s->name);
            }

            s->substage = A_STATE_SUBSTAGE_PAUSE;

            a_out__state("'%s' going from %s/%s to %s/%s",
                s->name,
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[A_STATE_SUBSTAGE_RUN],
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[A_STATE_SUBSTAGE_PAUSE]);
        } break;

        case A_STATE_ACTION_RESUME: {
            AStateInstance* s = a_list_peek(g_stack);

            if(s == NULL) {
                a_out__fatal("Resume state: stack is empty");
            } else if(s->stage != A_STATE_STAGE_BODY) {
                a_out__fatal("Resume state '%s': only call from A_STATE_BODY",
                    s->name);
            } else if(s->substage != A_STATE_SUBSTAGE_PAUSE) {
                a_out__fatal("Resume state '%s': only call from A_STATE_PAUSE",
                    s->name);
            }

            s->substage = A_STATE_SUBSTAGE_RUN;

            a_out__state("'%s' going from %s/%s to %s/%s",
                s->name,
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[A_STATE_SUBSTAGE_PAUSE],
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[A_STATE_SUBSTAGE_RUN]);
        } break;

        case A_STATE_ACTION_EXIT: {
            a_out__state("Telling all states to exit");

            A_LIST_ITERATE(g_stack, AStateInstance, s) {
                if(s->stage == A_STATE_STAGE_BODY) {
                    a_out__state("'%s' going from %s/%s to %s",
                        s->name,
                        g_stageNames[s->stage],
                        g_subStageNames[s->substage],
                        g_stageNames[A_STATE_STAGE_FREE]);
                } else {
                    a_out__state("'%s' going from %s to %s",
                        s->name,
                        g_stageNames[s->stage],
                        g_stageNames[A_STATE_STAGE_FREE]);
                }

                s->stage = A_STATE_STAGE_FREE;
                s->substage = A_STATE_SUBSTAGE_INVALID;
            }
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
    a_out__state("Declared '%s'", Name);
}

void a_state_push(const char* Name)
{
    pending_new(A_STATE_ACTION_PUSH, Name);
}

void a_state_pop(void)
{
    pending_new(A_STATE_ACTION_POP, NULL);
}

void a_state_replace(const char* Name)
{
    a_state_pop();
    a_state_push(Name);
}

void a_state_pause(void)
{
    pending_new(A_STATE_ACTION_PAUSE, NULL);
}

void a_state_resume(void)
{
    pending_new(A_STATE_ACTION_RESUME, NULL);
}

void a_state_exit(void)
{
    pending_new(A_STATE_ACTION_EXIT, NULL);
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

bool a_state__substage(AStateSubStage Substage)
{
    const AStateInstance* current = a_list_peek(g_stack);

    if(current != NULL) {
        return current->substage == Substage;
    }

    return false;
}

bool a_state__nothingPending(void)
{
    static bool first = true;

    if(first) {
        first = false;
    } else {
        a_fps_end();
    }

    a_fps_start();

    if(a_list_isEmpty(g_pending)) {
        return true;
    } else {
        first = true;
        a_fps_end();

        return false;
    }
}

void a_state__run(void)
{
    state_handle();

    while(!a_list_isEmpty(g_stack)) {
        AStateInstance* s = a_list_peek(g_stack);

        if(s->stage == A_STATE_STAGE_BODY) {
            a_out__state("'%s' running %s/%s",
                s->name,
                g_stageNames[A_STATE_STAGE_BODY],
                g_subStageNames[s->substage]);
        } else {
            a_out__state("'%s' running %s",
                s->name,
                g_stageNames[s->stage]);
        }

        s->function();

        // Check if the state just ran its Free stage
        if(s->stage == A_STATE_STAGE_FREE) {
            state_free(s);
            a_list_pop(g_stack);
        }

        state_handle();
    }
}
