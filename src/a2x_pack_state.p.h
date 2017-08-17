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

#pragma once

#include "a2x_system_includes.h"

typedef void (*AStateFunction)(void);

#define A_STATE__NAME(Name) a_state__function_##Name

extern void a_state__new(const char* Name, AStateFunction Function, const char* TickSystems, const char* DrawSystems);
#define a_state_new(Name, Function, TickSystems, DrawSystems) \
    a_state__new(Name, A_STATE__NAME(Function), TickSystems, DrawSystems)

extern void a_state_push(const char* Name);
extern void a_state_pop(void);
extern void a_state_popUntil(const char* Name);
extern void a_state_replace(const char* Name);
extern void a_state_exit(void);

typedef enum {
    A_STATE_STAGE_INVALID,
    A_STATE_STAGE_INIT,
    A_STATE_STAGE_BODY,
    A_STATE_STAGE_FREE,
    A_STATE_STAGE_NUM
} AStateStage;

#define A_STATE(Name) void A_STATE__NAME(Name)(void)

#define A_STATE_INIT if(a_state__stage(A_STATE_STAGE_INIT))
#define A_STATE_BODY if(a_state__stage(A_STATE_STAGE_BODY))
#define A_STATE_LOOP while(a_state__loop())
#define A_STATE_LOOP_DRAW if(a_fps__notSkipped())
#define A_STATE_FREE if(a_state__stage(A_STATE_STAGE_FREE))

extern bool a_state__stage(AStateStage Stage);
extern bool a_state__loop(void);
