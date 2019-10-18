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

#ifndef A_INC_GENERAL_STATE_P_H
#define A_INC_GENERAL_STATE_P_H

#include "general/a_system_includes.h"

typedef void AStateHandler(void);

extern void a_state_push(AStateHandler* Handler, const char* Name);
extern void a_state_pop(void);
extern void a_state_popUntil(AStateHandler* Handler, const char* Name);
extern void a_state_replace(AStateHandler* Handler, const char* Name);

#ifndef FAUR_IMPLEMENT
    #define a_state_push(Handler) a_state_push(Handler, #Handler)
    #define a_state_popUntil(Handler) a_state_popUntil(Handler, #Handler)
    #define a_state_replace(Handler) a_state_replace(Handler, #Handler)
#endif

extern void a_state_exit(void);

extern AStateHandler* a_state_currentGet(void);
extern bool a_state_currentChanged(void);

extern bool a_state_blockGet(void);
extern void a_state_blockSet(const AEvent* Event);

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

#endif // A_INC_GENERAL_STATE_P_H
