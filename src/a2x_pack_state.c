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

typedef void (*StateFunction)(void);

struct StateInstance {
    char* name;
    StateFunction function;
    StrHash* objects;
    StateStage stage;
    StateBodyStage bodystage;
};

static StrHash* functions;
static List* stack;
static StateInstance* new_state;
static bool changed;
static bool replacing;
static int indent;

static char* stage_names[A_STATE_STAGE_NUM] = {
    "Invalid",
    "Init",
    "Body",
    "Free",
};

static char* bodystage_names[A_STATE_BODYSTAGE_NUM] = {
    "Invalid",
    "Running",
    "Paused",
};

static StateInstance* a_stateinstance__new(const char* name)
{
    StateFunction function = a_strhash_get(functions, name);

    if(function == NULL) {
        a_out__fatal("State '%s' does not exist", name);
    }

    StateInstance* const s = a_mem_malloc(sizeof(StateInstance));

    s->name = a_str_dup(name);
    s->function = function;
    s->objects = a_strhash_new();
    s->stage = A_STATE_STAGE_INIT;
    s->bodystage = A_STATE_BODYSTAGE_RUN;

    return s;
}

static void a_stateinstance__free(StateInstance* s)
{
    free(s->name);
    a_strhash_free(s->objects);

    free(s);
}

void a_state__init(void)
{
    functions = a_strhash_new();
    stack = a_list_new();
    new_state = NULL;
    changed = false;
    replacing = false;
    indent = 0;
}

void a_state__uninit(void)
{
    a_strhash_free(functions);

    A_LIST_ITERATE(stack, StateInstance, s) {
        a_stateinstance__free(s);
    }
    a_list_free(stack);
}

void a_state__new(const char* name, void (*function)(void))
{
    a_out__state("New state '%s'", name);
    a_strhash_add(functions, name, function);
}

void a_state_push(const char* name)
{
    const StateInstance* const active = a_list_peek(stack);

    if(active && !replacing) {
        if(active->stage == A_STATE_STAGE_FREE) {
            a_out__state("Push state '%s': already exiting", name);
            return;
        } else if(active->stage != A_STATE_STAGE_BODY) {
            a_out__fatal("Push state '%s': only call from A_STATE_BODY", name);
        }
    }

    changed = true;

    StateInstance* const s = a_stateinstance__new(name);

    if(a_list_isEmpty(stack)) {
        a_out__state("Push first state '%s'", name);
        a_list_push(stack, s);
    } else if(new_state == NULL) {
        a_out__state("Push state '%s'", name);
        new_state = s;
    } else {
        a_out__fatal("Push state '%s': already pushed state '%s'", name, new_state->name);
    }
}

void a_state_pop(void)
{
    StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_out__fatal("Pop state: no active state");
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_out__state("Pop state '%s': already exiting", active->name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_out__fatal("Pop state '%s': only call from A_STATE_BODY", active->name);
    }

    a_out__state("Pop state '%s'", active->name);

    changed = true;

    indent++;
    a_state__setStage(active, A_STATE_STAGE_FREE);
    indent--;
}

void a_state_replace(const char* name)
{
    if(a_strhash_get(functions, name) == NULL) {
        a_out__fatal("Replace state '%s': does not exist", name);
    }

    const StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_out__fatal("Replace state with '%s': no active state, use a_state_push", name);
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_out__state("Replace state '%s' with '%s': already exiting", active->name, name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_out__fatal("Replace state '%s' with '%s': only call from A_STATE_BODY", active->name, name);
    }

    a_out__state("Replace state '%s' with '%s'", active->name, name);

    replacing = true;
    indent++;

    a_state_pop();
    a_state_push(name);

    replacing = false;
    indent--;
}

void a_state_pause(void)
{
    StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_out__fatal("No active state to pause");
    } else if(active->bodystage == A_STATE_BODYSTAGE_PAUSE) {
        a_out__fatal("State '%s' is already paused", active->name);
    }

    a_state__setBodyStage(active, A_STATE_BODYSTAGE_PAUSE);
    changed = true;
}

void a_state_resume(void)
{
    StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_out__fatal("No active state to resume");
    } else if(active->bodystage == A_STATE_BODYSTAGE_RUN) {
        a_out__fatal("State '%s' is already running", active->name);
    }

    a_state__setBodyStage(active, A_STATE_BODYSTAGE_RUN);
    changed = true;
}

void a_state_exit(void)
{
    changed = true;
    a_out__state("Telling all states to exit");

    A_LIST_ITERATE(stack, StateInstance, s) {
        a_out__state("State '%s' exiting", s->name);

        indent++;
        a_state__setStage(s, A_STATE_STAGE_FREE);
        indent--;
    }
}

void a_state_add(const char* name, void* object)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        a_strhash_add(s->objects, name, object);
    }
}

void* a_state_get(const char* name)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        return a_strhash_get(s->objects, name);
    }

    return NULL;
}

void a_state__run(void)
{
    while(!a_list_isEmpty(stack)) {
        const StateInstance* const s = a_list_peek(stack);

        changed = false;
        s->function();

        if(s->stage == A_STATE_STAGE_FREE) {
            a_stateinstance__free(a_list_pop(stack));
        }

        if(new_state) {
            a_list_push(stack, new_state);
            new_state = NULL;
        }
    }
}

StateStage a_state__stage(void)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        return s->stage;
    }

    return A_STATE_STAGE_INVALID;
}

StateBodyStage a_state__bodystage(void)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        return s->bodystage;
    }

    return A_STATE_BODYSTAGE_INVALID;
}

bool a_state__setStage(StateInstance* state, StateStage stage)
{
    StateInstance* const s = state ? state : a_list_peek(stack);

    if(s) {
        a_out__state("State '%s' transitioning from %s to %s",
            s->name, stage_names[s->stage], stage_names[stage]);
        s->stage = stage;

        return true;
    }

    return false;
}

void a_state__setBodyStage(StateInstance* state, StateBodyStage bodystage)
{
    a_out__state("State '%s' transitioning from %s to %s",
        state->name, bodystage_names[state->bodystage], bodystage_names[bodystage]);
    state->bodystage = bodystage;
}

bool a_state__unchanged(void)
{
    static bool first = true;

    if(first) {
        first = false;
    } else {
        a_fps_end();
    }

    a_fps_start();

    if(changed) {
        first = true;
        a_fps_end();
    }

    return !changed;
}
