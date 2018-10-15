/*
    Copyright 2010, 2016-2018 Alex Margarit

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

typedef enum {
    A_STATE__STAGE_INVALID = -1,
    A_STATE__STAGE_INIT,
    A_STATE__STAGE_LOOP,
    A_STATE__STAGE_FREE,
    A_STATE__STAGE_NUM
} AStateStage;

typedef struct AState AState;

typedef void AStateFunction(AStateStage A__Stage, bool Tick);

#define A_STATE(Name) void Name(AStateStage A__Stage, bool A__Tick)

extern AState* a_state_new(const char* Name, AStateFunction* Function);

extern void a_state_push(AState* State);
extern void a_state_pop(void);
extern void a_state_popUntil(AState* State);
extern void a_state_replace(AState* State);
extern void a_state_exit(void);

#define A_STATE_INIT if(A__Stage == A_STATE__STAGE_INIT)
#define A_STATE_TICK if(A__Stage == A_STATE__STAGE_LOOP && A__Tick)
#define A_STATE_DRAW if(A__Stage == A_STATE__STAGE_LOOP && !A__Tick)
#define A_STATE_FREE if(A__Stage == A_STATE__STAGE_FREE)
