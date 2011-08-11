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
    StateFunction function;
} StateInstance;

static Hash* functions;
static List* stack;

static bool changed;

void a_state__init(void)
{
    functions = a_hash_new();
    stack = a_list_new();

    changed = false;
}

void a_state__free(void)
{
    a_hash_free(functions);
    a_list_free(stack);
}

void a_state__run(void)
{
    while(!a_list_isEmpty(stack)) {
        changed = false;
        ((StateInstance*)a_list_peek(stack))->function();
    }
}

void a_state_add(const char* const name, void (*function)(void))
{
    a_hash_add(functions, name, function);
}

void a_state_push(const char* const name)
{
    StateFunction function = a_hash_get(functions, name);

    if(function == NULL) {
        a_error("No state '%s'", name);
        return;
    }

    changed = true;

    StateInstance* const s = malloc(sizeof(StateInstance));

    s->function = function;

    a_list_push(stack, s);
}

void a_state_pop(void)
{
    changed = true;

    StateInstance* const s = a_list_pop(stack);

    free(s);
}

void a_state_replace(const char* const name)
{
    if(a_hash_get(functions, name) == NULL) {
        a_error("No state '%s'", name);
        return;
    }

    a_state_pop();
    a_state_push(name);
}

void a_state_exit(void)
{
    changed = true;
    a_list_empty(stack);
}

bool a_state_unchanged(void)
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
