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

#pragma once

#include "a2x_system_includes.h"

typedef struct AStateInstance AStateInstance;
typedef void (*AStateFunction)(void);

#define A_STATE__MAKE_NAME(Name) a_state__function_##Name

#define a_state_new(Name, Function) a_state__new(Name, A_STATE__MAKE_NAME(Function))
extern void a_state__new(const char* Name, void (*Function)(void));

extern void a_state_push(const char* Name);
extern void a_state_pop(void);
extern void a_state_replace(const char* Name);
extern void a_state_pause(void);
extern void a_state_resume(void);
extern void a_state_exit(void);

extern void a_state_add(const char* Name, void* Object);
extern void* a_state_get(const char* Name);

typedef enum {
    A_STATE_STAGE_INVALID,
    A_STATE_STAGE_INIT,
    A_STATE_STAGE_BODY,
    A_STATE_STAGE_FREE,
    A_STATE_STAGE_NUM
} AStateStage;

typedef enum {
    A_STATE_SUBSTAGE_INVALID,
    A_STATE_SUBSTAGE_RUN,
    A_STATE_SUBSTAGE_PAUSE,
    A_STATE_SUBSTAGE_NUM
} AStateSubStage;

#define A_STATE(Name)                   \
    void A_STATE__MAKE_NAME(Name)(void)

#define A_STATE_INIT                       \
    if(a_state__stage(A_STATE_STAGE_INIT))

#define A_STATE_BODY                       \
    if(a_state__stage(A_STATE_STAGE_BODY))

#define A_STATE_RUN                                 \
    if(a_state__stage(A_STATE_STAGE_BODY)           \
        && a_state__substage(A_STATE_SUBSTAGE_RUN))

#define A_STATE_PAUSE                                 \
    if(a_state__stage(A_STATE_STAGE_BODY)             \
        && a_state__substage(A_STATE_SUBSTAGE_PAUSE))

#define A_STATE_LOOP            \
    while(a_state__nothingPending())

#define A_STATE_FREE                       \
    if(a_state__stage(A_STATE_STAGE_FREE))

extern bool a_state__stage(AStateStage Stage);
extern bool a_state__substage(AStateSubStage BodyStage);

extern bool a_state__nothingPending(void);
