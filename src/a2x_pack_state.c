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

typedef void (*AStateFunction)(void);

struct AStateInstance {
    char* name;
    AStateFunction function;
    AStrHash* objects;
    AStateStage stage;
    AStateBodyStage bodystage;
};

static AStrHash* g_states;
static AList* g_statesStack;
static AStateInstance* g_nextNewState;
static bool g_somethingChanged;
static bool g_replacingState;
static int g_outputIndent;

static char* g_stageNames[A_STATE_STAGE_NUM] = {
    "Invalid",
    "Init",
    "Body",
    "Free",
};

static char* g_bodyStageNames[A_STATE_BODYSTAGE_NUM] = {
    "Invalid",
    "Running",
    "Paused",
};

static AStateInstance* a_stateinstance__new(const char* Name)
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
    s->bodystage = A_STATE_BODYSTAGE_RUN;

    return s;
}

static void a_stateinstance__free(AStateInstance* State)
{
    free(State->name);
    a_strhash_free(State->objects);

    free(State);
}

void a_state__init(void)
{
    g_states = a_strhash_new();
    g_statesStack = a_list_new();
    g_nextNewState = NULL;
    g_somethingChanged = false;
    g_replacingState = false;
    g_outputIndent = 0;
}

void a_state__uninit(void)
{
    a_strhash_free(g_states);

    A_LIST_ITERATE(g_statesStack, AStateInstance, s) {
        a_stateinstance__free(s);
    }
    a_list_free(g_statesStack);
}

void a_state__new(const char* Name, void (*Function)(void))
{
    a_out__state("New state '%s'", Name);
    a_strhash_add(g_states, Name, Function);
}

void a_state_push(const char* Name)
{
    const AStateInstance* const active = a_list_peek(g_statesStack);

    if(active && !g_replacingState) {
        if(active->stage == A_STATE_STAGE_FREE) {
            a_out__state("Push state '%s': already exiting", Name);
            return;
        } else if(active->stage != A_STATE_STAGE_BODY) {
            a_out__fatal("Push state '%s': only call from A_STATE_BODY", Name);
        }
    }

    g_somethingChanged = true;

    AStateInstance* const s = a_stateinstance__new(Name);

    if(a_list_isEmpty(g_statesStack)) {
        a_out__state("Push first state '%s'", Name);
        a_list_push(g_statesStack, s);
    } else if(g_nextNewState == NULL) {
        a_out__state("Push state '%s'", Name);
        g_nextNewState = s;
    } else {
        a_out__fatal("Push state '%s': already pushed state '%s'", Name, g_nextNewState->name);
    }
}

void a_state_pop(void)
{
    AStateInstance* const active = a_list_peek(g_statesStack);

    if(active == NULL) {
        a_out__fatal("Pop state: no active state");
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_out__state("Pop state '%s': already exiting", active->name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_out__fatal("Pop state '%s': only call from A_STATE_BODY", active->name);
    }

    a_out__state("Pop state '%s'", active->name);

    g_somethingChanged = true;

    g_outputIndent++;
    a_state__setStage(active, A_STATE_STAGE_FREE);
    g_outputIndent--;
}

void a_state_replace(const char* Name)
{
    if(a_strhash_get(g_states, Name) == NULL) {
        a_out__fatal("Replace state '%s': does not exist", Name);
    }

    const AStateInstance* const active = a_list_peek(g_statesStack);

    if(active == NULL) {
        a_out__fatal("Replace state with '%s': no active state, use a_state_push", Name);
    } else if(active->stage == A_STATE_STAGE_FREE) {
        a_out__state("Replace state '%s' with '%s': already exiting", active->name, Name);
        return;
    } else if(active->stage != A_STATE_STAGE_BODY) {
        a_out__fatal("Replace state '%s' with '%s': only call from A_STATE_BODY", active->name, Name);
    }

    a_out__state("Replace state '%s' with '%s'", active->name, Name);

    g_replacingState = true;
    g_outputIndent++;

    a_state_pop();
    a_state_push(Name);

    g_replacingState = false;
    g_outputIndent--;
}

void a_state_pause(void)
{
    AStateInstance* const active = a_list_peek(g_statesStack);

    if(active == NULL) {
        a_out__fatal("No active state to pause");
    } else if(active->bodystage == A_STATE_BODYSTAGE_PAUSE) {
        a_out__fatal("State '%s' is already paused", active->name);
    }

    a_state__setBodyStage(active, A_STATE_BODYSTAGE_PAUSE);
    g_somethingChanged = true;
}

void a_state_resume(void)
{
    AStateInstance* const active = a_list_peek(g_statesStack);

    if(active == NULL) {
        a_out__fatal("No active state to resume");
    } else if(active->bodystage == A_STATE_BODYSTAGE_RUN) {
        a_out__fatal("State '%s' is already running", active->name);
    }

    a_state__setBodyStage(active, A_STATE_BODYSTAGE_RUN);
    g_somethingChanged = true;
}

void a_state_exit(void)
{
    g_somethingChanged = true;
    a_out__state("Telling all states to exit");

    A_LIST_ITERATE(g_statesStack, AStateInstance, s) {
        a_out__state("State '%s' exiting", s->name);

        g_outputIndent++;
        a_state__setStage(s, A_STATE_STAGE_FREE);
        g_outputIndent--;
    }
}

void a_state_add(const char* Name, void* Object)
{
    const AStateInstance* const s = a_list_peek(g_statesStack);

    if(s) {
        a_strhash_add(s->objects, Name, Object);
    }
}

void* a_state_get(const char* Name)
{
    const AStateInstance* const s = a_list_peek(g_statesStack);

    if(s) {
        return a_strhash_get(s->objects, Name);
    }

    return NULL;
}

void a_state__run(void)
{
    while(!a_list_isEmpty(g_statesStack)) {
        const AStateInstance* const s = a_list_peek(g_statesStack);

        g_somethingChanged = false;
        s->function();

        if(s->stage == A_STATE_STAGE_FREE) {
            a_stateinstance__free(a_list_pop(g_statesStack));
        }

        if(g_nextNewState) {
            a_list_push(g_statesStack, g_nextNewState);
            g_nextNewState = NULL;
        }
    }
}

AStateStage a_state__stage(void)
{
    const AStateInstance* const s = a_list_peek(g_statesStack);

    if(s) {
        return s->stage;
    }

    return A_STATE_STAGE_INVALID;
}

AStateBodyStage a_state__bodystage(void)
{
    const AStateInstance* const s = a_list_peek(g_statesStack);

    if(s) {
        return s->bodystage;
    }

    return A_STATE_BODYSTAGE_INVALID;
}

bool a_state__setStage(AStateInstance* State, AStateStage Stage)
{
    if(State == NULL) {
        State = a_list_peek(g_statesStack);
    }

    if(State) {
        a_out__state("State '%s' transitioning from %s to %s",
            State->name, g_stageNames[State->stage], g_stageNames[Stage]);
        State->stage = Stage;

        return true;
    }

    return false;
}

void a_state__setBodyStage(AStateInstance* State, AStateBodyStage Bodystage)
{
    a_out__state("State '%s' transitioning from %s to %s",
        State->name, g_bodyStageNames[State->bodystage], g_bodyStageNames[Bodystage]);
    State->bodystage = Bodystage;
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

    if(g_somethingChanged) {
        first = true;
        a_fps_end();
    }

    return !g_somethingChanged;
}
