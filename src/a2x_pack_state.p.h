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

#define A_STATE(Name) void Name(void)
typedef A_STATE(AState);

extern void a_state_push(AState* State, const char* Name);
extern void a_state_pop(void);
extern void a_state_popUntil(AState* State, const char* Name);
extern void a_state_replace(AState* State, const char* Name);

extern void a_state_exit(void);

extern bool a_state_blockGet(void);
extern void a_state_blockSet(AEvent* Event);

typedef enum {
    A__STATE_STAGE_INVALID = -1,
    A__STATE_STAGE_INIT,
    A__STATE_STAGE_TICK,
    A__STATE_STAGE_DRAW,
    A__STATE_STAGE_FREE,
    A__STATE_STAGE_NUM
} AStateStage;

extern bool a__state_stageCheck(AStateStage Stage);

#define A_STATE_INIT if(a__state_stageCheck(A__STATE_STAGE_INIT))
#define A_STATE_TICK if(a__state_stageCheck(A__STATE_STAGE_TICK))
#define A_STATE_DRAW if(a__state_stageCheck(A__STATE_STAGE_DRAW))
#define A_STATE_FREE if(a__state_stageCheck(A__STATE_STAGE_FREE))
