/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_STATE_P_H
#define F_INC_GENERAL_STATE_P_H

#include "../general/f_system_includes.h"

typedef void FState(void);
typedef void FStateCallback(void);

extern void f_state_callbacks(FStateCallback* TickPre, FStateCallback* TickPost, FStateCallback* DrawPre, FStateCallback* DrawPost);

extern void f_state_push(FState* Handler, const char* Name);
extern void f_state_pop(void);
extern void f_state_popUntil(FState* Handler, const char* Name);
extern void f_state_replace(FState* Handler, const char* Name);

#ifndef FAUR_IMPLEMENT
    #define f_state_push(Handler) f_state_push(Handler, #Handler)
    #define f_state_popUntil(Handler) f_state_popUntil(Handler, #Handler)
    #define f_state_replace(Handler) f_state_replace(Handler, #Handler)
#endif

extern void f_state_exit(void);

extern FState* f_state_currentGet(void);
extern bool f_state_currentChanged(void);

extern bool f_state_blockGet(void);
extern void f_state_blockSet(const FEvent* Event);

typedef enum {
    F__STATE_STAGE_INVALID = -1,
    F__STATE_STAGE_INIT,
    F__STATE_STAGE_TICK,
    F__STATE_STAGE_DRAW,
    F__STATE_STAGE_FREE,
    F__STATE_STAGE_NUM
} FStateStage;

extern bool f__state_stageCheck(FStateStage Stage);

#define F_STATE_INIT if(f__state_stageCheck(F__STATE_STAGE_INIT))
#define F_STATE_TICK if(f__state_stageCheck(F__STATE_STAGE_TICK))
#define F_STATE_DRAW if(f__state_stageCheck(F__STATE_STAGE_DRAW))
#define F_STATE_FREE if(f__state_stageCheck(F__STATE_STAGE_FREE))

#endif // F_INC_GENERAL_STATE_P_H
