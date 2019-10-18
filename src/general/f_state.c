/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define FAUR_IMPLEMENT 1

#include "f_state.v.h"
#include <faur.v.h>

#if F_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

typedef struct {
    const char* name;
    AStateHandler* handler;
    AStateStage stage;
} AStateStackEntry;

static AList* g_stack; // list of AStateStackEntry
static AList* g_pending; // list of AStateStackEntry/NULL
static bool g_exiting;
static const AEvent* g_blockEvent;

#if F_CONFIG_BUILD_DEBUG
static const char* g_stageNames[F__STATE_STAGE_NUM] = {
    [F__STATE_STAGE_INIT] = "Init",
    [F__STATE_STAGE_TICK] = "Loop",
    [F__STATE_STAGE_DRAW] = "Loop",
    [F__STATE_STAGE_FREE] = "Free",
};
#endif

static void pending_push(AStateHandler* Handler, const char* Name)
{
    AStateStackEntry* e = f_mem_malloc(sizeof(AStateStackEntry));

    e->name = Name;
    e->handler = Handler;
    e->stage = F__STATE_STAGE_INIT;

    f_list_addLast(g_pending, e);
}

static void pending_pop(void)
{
    f_list_addLast(g_pending, NULL);
}

static void pending_handle(void)
{
    AStateStackEntry* current = f_list_peek(g_stack);

    // Check if the current state just ran its Free stage
    if(current && current->stage == F__STATE_STAGE_FREE) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("Destroying '%s' instance", current->name);
        #endif

        f_mem_free(f_list_pop(g_stack));
        current = f_list_peek(g_stack);

        if(!g_exiting && f_list_isEmpty(g_pending)
            && current && current->stage == F__STATE_STAGE_TICK) {

            f_fps__reset();
        }
    }

    // If there are no pending state changes,
    // check if the current state should transition from Init to Loop
    if(f_list_isEmpty(g_pending)) {
        if(current && current->stage == F__STATE_STAGE_INIT) {
            #if F_CONFIG_BUILD_DEBUG
                f_out__state("'%s' going from %s to %s",
                             current->name,
                             g_stageNames[F__STATE_STAGE_INIT],
                             g_stageNames[F__STATE_STAGE_TICK]);
            #endif

            current->stage = F__STATE_STAGE_TICK;

            f_fps__reset();
        }

        return;
    }

    AStateStackEntry* pendingState = f_list_pop(g_pending);

    if(pendingState == NULL) {
        #if F_CONFIG_BUILD_DEBUG
            if(current == NULL) {
                F__FATAL("Pop state: stack is empty");
            }

            f_out__state("Pop '%s'", current->name);

            f_out__state("'%s' going from %s to %s",
                         current->name,
                         g_stageNames[current->stage],
                         g_stageNames[F__STATE_STAGE_FREE]);
        #endif

        current->stage = F__STATE_STAGE_FREE;
    } else {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("Push '%s'", pendingState->name);

            F_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
                if(pendingState->handler == e->handler) {
                    F__FATAL("State '%s' already in stack", e->name);
                }
            }
        #endif

        f_out__state("New '%s' instance", pendingState->name);
        f_list_push(g_stack, pendingState);
    }
}

static void f_state__init(void)
{
    g_stack = f_list_new();
    g_pending = f_list_new();
}

static void f_state__uninit(void)
{
    f_list_freeEx(g_stack, f_mem_free);
    f_list_freeEx(g_pending, f_mem_free);
}

const APack f_pack__state = {
    "State",
    {
        [0] = f_state__init,
    },
    {
        [0] = f_state__uninit,
    },
};

void f_state_push(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("f_state_push(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        f_out__state("f_state_push(%s)", Name);
    #endif

    pending_push(Handler, Name);
}

void f_state_pop(void)
{
    if(g_exiting) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("f_state_pop: Already exiting");
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        f_out__state("f_state_pop()");
    #endif

    pending_pop();
}

void f_state_popUntil(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("f_state_popUntil(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        f_out__state("f_state_popUntil(%s)", Name);
    #endif

    int pops = 0;
    bool found = false;

    F_LIST_ITERATE(g_stack, const AStateStackEntry*, e) {
        if(e->handler == Handler) {
            found = true;
            break;
        }

        pops++;
    }

    if(!found) {
        F__FATAL("f_state_popUntil(%s): State not in stack", Name);
    }

    while(pops--) {
        pending_pop();
    }
}

void f_state_replace(AStateHandler* Handler, const char* Name)
{
    if(g_exiting) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("f_state_replace(%s): Already exiting", Name);
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        f_out__state("f_state_replace(%s)", Name);
    #endif

    pending_pop();
    pending_push(Handler, Name);
}

void f_state_exit(void)
{
    if(g_exiting) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("f_state_exit: Already exiting");
        #endif

        return;
    }

    f_out__state("*** Telling all states to exit ***");

    g_exiting = true;

    // Clear the pending actions queue
    f_list_clearEx(g_pending, f_mem_free);

    // Queue a pop for every state in the stack
    for(unsigned i = f_list_sizeGet(g_stack); i--; ) {
        pending_pop();
    }
}

AStateHandler* f_state_currentGet(void)
{
    AStateStackEntry* current = f_list_peek(g_stack);

    if(current == NULL) {
        F__FATAL("f_state_currentGet: Stack is empty");
    }

    return current->handler;
}

bool f_state_currentChanged(void)
{
    return !f_list_isEmpty(g_pending);
}

bool f_state_blockGet(void)
{
    if(g_blockEvent) {
        if(*g_blockEvent != 0) {
            return true;
        }

        g_blockEvent = NULL;
    }

    return false;
}

void f_state_blockSet(const AEvent* Event)
{
    g_blockEvent = Event;
}

bool f_state__runStep(void)
{
    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        if(!EM_ASM_INT({ return Module.faur_fsIsReady; }, 0)) {
            return true;
        }
    #endif

    if(!f_state_blockGet()) {
        pending_handle();
    }

    AStateStackEntry* s = f_list_peek(g_stack);

    if(s == NULL) {
        return false;
    }

    if(s->stage == F__STATE_STAGE_TICK) {
        while(f_fps__tick()) {
            f_timer__tick();
            f_input__tick();
            f_sound__tick();
            f_screen__tick();
            f_screenshot__tick();
            f_console__tick();
            f_ecs__tick();
            f_fade__tick();

            if(!f_list_isEmpty(g_pending) && !f_state_blockGet()) {
                return true;
            }

            s->handler();

            if(!f_list_isEmpty(g_pending) && !f_state_blockGet()) {
                return true;
            }
        }

        s->stage = F__STATE_STAGE_DRAW;
        s->handler();
        s->stage = F__STATE_STAGE_TICK;

        f_fade__draw();
        f_sound__draw();
        f_console__draw();
        f_screen__draw();

        f_fps__frame();
    } else {
        #if F_CONFIG_BUILD_DEBUG
            f_out__state("'%s' running %s", s->name, g_stageNames[s->stage]);
        #endif

        s->handler();
    }

    return true;
}

#if F_CONFIG_SYSTEM_EMSCRIPTEN
static void loop(void)
{
    if(!f_state__runStep()) {
        f_out__state("Finished running states");
        emscripten_cancel_main_loop();
    }
}
#endif

void f_state__runLoop(void)
{
    f_out__state("Running states");

    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        emscripten_set_main_loop(
            loop,
            f_platform_api__screenVsyncGet() ? 0 : F_CONFIG_FPS_RATE_DRAW,
            true);
    #else
        while(f_state__runStep()) {
            continue;
        }

        f_out__state("Finished running states");
    #endif
}

bool f__state_stageCheck(AStateStage Stage)
{
    const AStateStackEntry* e = f_list_peek(g_stack);

    #if F_CONFIG_BUILD_DEBUG
        if(e == NULL) {
            F__FATAL("%s: state stack is empty", g_stageNames[Stage]);
        }
    #endif

    return e->stage == Stage;
}
