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

typedef enum {
    A_STATE__STAGE_INIT = 1,
    A_STATE__STAGE_LOOP = 2,
    A_STATE__STAGE_TICK = 4,
    A_STATE__STAGE_DRAW = 8,
    A_STATE__STAGE_FREE = 16,
} AStateStage;

typedef void (*AStateFunction)(AStateStage A__stage);
#define A_STATE(Name) void A_STATE__NAME(Name)(AStateStage A__stage)

#define A_STATE__NAME(Name) a_state__function_##Name

extern void a_state__new(const char* Name, AStateFunction Function);
#define a_state_new(Name, Function) a_state__new(Name, A_STATE__NAME(Function))

extern void a_state_push(const char* Name);
extern void a_state_pop(void);
extern void a_state_popUntil(const char* Name);
extern void a_state_replace(const char* Name);
extern void a_state_exit(void);

#define A_STATE_INIT if(A__stage & A_STATE__STAGE_INIT)
#define A_STATE_TICK if(A__stage & A_STATE__STAGE_TICK)
#define A_STATE_DRAW if(A__stage & A_STATE__STAGE_DRAW)
#define A_STATE_FREE if(A__stage & A_STATE__STAGE_FREE)
