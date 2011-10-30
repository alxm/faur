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

#ifndef A2X_PACK_STATE_PH
#define A2X_PACK_STATE_PH

#include "a2x_app_includes.h"

extern void a_state_new(const char* name, void (*function)(void));

extern void a_state_push(const char* name);
extern void a_state_pop(void);

extern void a_state_replace(const char* name);
extern void a_state_exit(void);

typedef enum {A_STATE_STAGE_INVALID, A_STATE_STAGE_INIT, A_STATE_STAGE_BODY, A_STATE_STAGE_FREE} StateStage;

#define A_STATE(state_name) void state_name(void)
#define A_STATE_INIT if(a_state__stage() == A_STATE_STAGE_INIT)
#define A_STATE_BODY if(a_state__stage() == A_STATE_STAGE_BODY || (a_state__stage() == A_STATE_STAGE_INIT && a_state__setStage(A_STATE_STAGE_BODY)))
#define A_STATE_FREE if(a_state__stage() == A_STATE_STAGE_FREE)
#define A_STATE_LOOP while(a_state__unchanged())

extern void a_state_add(const char* name, void* object);
extern void* a_state_get(const char* name);

extern StateStage a_state__stage(void);
extern bool a_state__setStage(StateStage stage);
extern bool a_state__unchanged(void);

#endif // A2X_PACK_STATE_PH
