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

typedef struct StateInstance {
    const char* name;
    StateFunction function;
    Hash* objects;
    StateStage stage;
} StateInstance;

static Hash* functions;
static List* stack;
static StateInstance* new_state;
static bool changed;
static bool replacing;

void a_state__init(void)
{
    functions = a_hash_new();
    stack = a_list_new();
    new_state = NULL;
    changed = false;
    replacing = false;
}

void a_state__uninit(void)
{
    a_hash_free(functions, false);

    A_LIST_ITERATE(stack, StateInstance, s) {
        a_hash_free(s->objects, false);
        free(s);
    }

    a_list_free(stack, false);
}

void a_state_new(const char* const name, void (*function)(void))
{
    a_hash_add(functions, name, function);
}

void a_state_push(const char* const name)
{
    const StateInstance* const active = a_list_peek(stack);

    if(!replacing && active && active->stage != A_STATE_STAGE_BODY) {
        a_error("Push state '%s': only call from A_STATE_BODY", name);
        exit(1);
    }

    StateFunction function = a_hash_get(functions, name);

    if(function == NULL) {
        a_error("Push state '%s': does not exist", name);
        exit(1);
    }

    changed = true;

    StateInstance* const s = malloc(sizeof(StateInstance));

    s->name = a_str_dup(name);
    s->function = function;
    s->objects = a_hash_new();
    s->stage = A_STATE_STAGE_INIT;

    if(a_list_isEmpty(stack)) {
        a_list_push(stack, s);
    } else if(new_state == NULL) {
        new_state = s;
    } else {
        a_error("Push state '%s': already pushed state '%s'",
            name, new_state->name);
        exit(1);
    }
}

void a_state_pop(void)
{
    StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_error("Pop state: no active state");
        exit(1);
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_error("Pop state '%s': only call from A_STATE_BODY", active->name);
        exit(1);
    }

    changed = true;
    active->stage = A_STATE_STAGE_FREE;
}

void a_state_replace(const char* const name)
{
    if(a_hash_get(functions, name) == NULL) {
        a_error("Replace state '%s': does not exist", name);
        exit(1);
    }

    const StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_error("Replace state '%s': no active state, use a_state_push", name);
        exit(1);
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_error(
            "Replace state '%s' with '%s': only call from A_STATE_BODY",
            active->name, name);
        exit(1);
    }

    replacing = true;

    a_state_pop();
    a_state_push(name);

    replacing = false;
}

void a_state_exit(void)
{
    changed = true;

    A_LIST_ITERATE(stack, StateInstance, s) {
        s->stage = A_STATE_STAGE_FREE;
    }
}

void a_state_add(const char* const name, void* const object)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        a_hash_add(s->objects, name, object);
    }
}

void* a_state_get(const char* const name)
{
    const StateInstance* const s = a_list_peek(stack);

    if(s) {
        return a_hash_get(s->objects, name);
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
            a_hash_free(s->objects, false);
            free(a_list_pop(stack));
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

bool a_state__setStage(const StateStage stage)
{
    StateInstance* const s = a_list_peek(stack);

    if(s) {
        s->stage = stage;
        return true;
    }

    return false;
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
