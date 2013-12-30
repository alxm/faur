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
    StrHash* objects;
    StateStage stage;
} StateInstance;

static StrHash* functions;
static List* stack;
static StateInstance* new_state;
static bool changed;
static bool replacing;

#if A_PLATFORM_LINUXPC
    #define a_state__out(...)                                            \
    ({                                                                   \
        if(!a2x_bool("app.quiet")) {                                     \
            printf("%c[%d;%dm[ a2x Stt ]%c[%dm ", 0x1b, 0, 34, 0x1b, 0); \
            printf(__VA_ARGS__);                                         \
            printf("\n");                                                \
        }                                                                \
    })
#else
    #define a_state__out(...)        \
    ({                               \
        if(!a2x_bool("app.quiet")) { \
            printf("[ a2x Stt ] ");  \
            printf(__VA_ARGS__);     \
            printf("\n");            \
        }                            \
    })
#endif

void a_state__init(void)
{
    functions = a_strhash_new();
    stack = a_list_new();
    new_state = NULL;
    changed = false;
    replacing = false;
}

void a_state__uninit(void)
{
    a_strhash_free(functions);

    A_LIST_ITERATE(stack, StateInstance, s) {
        a_strhash_free(s->objects);
        free(s);
    }
    a_list_free(stack);
}

void a_state_new(const char* name, void (*function)(void))
{
    a_state__out("New state '%s'", name);
    a_strhash_add(functions, name, function);
}

void a_state_push(const char* name)
{
    const StateInstance* const active = a_list_peek(stack);

    if(active && !replacing) {
        if(active->stage == A_STATE_STAGE_FREE) {
            a_state__out("Push state '%s': already exiting", name);
            return;
        } else if(active->stage != A_STATE_STAGE_BODY) {
            a_error("Push state '%s': only call from A_STATE_BODY", name);
            exit(1);
        }
    }

    StateFunction function = a_strhash_get(functions, name);

    if(function == NULL) {
        a_error("Push state '%s': does not exist", name);
        exit(1);
    }

    changed = true;

    StateInstance* const s = malloc(sizeof(StateInstance));

    s->name = a_str_dup(name);
    s->function = function;
    s->objects = a_strhash_new();
    s->stage = A_STATE_STAGE_INIT;

    if(a_list_isEmpty(stack)) {
        a_state__out("Push first state '%s'", name);
        a_list_push(stack, s);
    } else if(new_state == NULL) {
        a_state__out("Push state '%s'", name);
        new_state = s;
    } else {
        a_error("Push state '%s': already pushed state '%s'", name, new_state->name);
        exit(1);
    }
}

void a_state_pop(void)
{
    StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_error("Pop state: no active state");
        exit(1);
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_state__out("Pop state '%s': already exiting", active->name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_error("Pop state '%s': only call from A_STATE_BODY", active->name);
        exit(1);
    }

    a_state__out("Pop state '%s'", active->name);

    changed = true;
    active->stage = A_STATE_STAGE_FREE;
}

void a_state_replace(const char* name)
{
    if(a_strhash_get(functions, name) == NULL) {
        a_error("Replace state '%s': does not exist", name);
        exit(1);
    }

    const StateInstance* const active = a_list_peek(stack);

    if(active == NULL) {
        a_error("Replace state with '%s': no active state, use a_state_push", name);
        exit(1);
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_state__out("Replace state '%s' with '%s': already exiting", active->name, name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_error("Replace state '%s' with '%s': only call from A_STATE_BODY", active->name, name);
        exit(1);
    }

    a_state__out("Replace state '%s' with '%s'", active->name, name);

    replacing = true;

    a_state_pop();
    a_state_push(name);

    replacing = false;
}

void a_state_exit(void)
{
    changed = true;

    A_LIST_ITERATE(stack, StateInstance, s) {
        a_state__out("State '%s' exiting", s->name);
        s->stage = A_STATE_STAGE_FREE;
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
            a_strhash_free(s->objects);
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

bool a_state__setStage(StateStage stage)
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
