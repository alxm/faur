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

extern void a_state_new(const char* const name, void (*function)(void));

extern void a_state_push(const char* const name);
extern void a_state_pop(void);

extern void a_state_replace(const char* const name);
extern void a_state_exit(void);

typedef enum {A_STATE_INVALID, A_STATE_INIT, A_STATE_BODY, A_STATE_FREE} StateStage;

#define State(state_name) void state_name(void)
#define StateInit if(a_state__stage() == A_STATE_INIT)
#define StateBody if(a_state__stage() == A_STATE_BODY || (a_state__stage() == A_STATE_INIT && a_state__setStage(A_STATE_BODY)))
#define StateFree if(a_state__stage() == A_STATE_FREE)
#define StateLoop while(a_state__stage() == A_STATE_BODY && a_state__unchanged())

extern void StateAdd(const char* const name, void* const object);
extern void* StateGet(const char* const name);

extern StateStage a_state__stage(void);
extern bool a_state__setStage(const StateStage stage);
extern bool a_state__unchanged(void);

#endif // A2X_PACK_STATE_PH
