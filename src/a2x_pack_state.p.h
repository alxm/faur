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

#include "a2x_app_includes.h"

typedef struct StateInstance StateInstance;

#define A_STATE__MAKE_NAME(name) a_state__function_##name

#define a_state_new(name, function) a_state__new(name, A_STATE__MAKE_NAME(function))
extern void a_state__new(const char* name, void (*function)(void));

extern void a_state_push(const char* name);
extern void a_state_pop(void);

extern void a_state_replace(const char* name);

extern void a_state_pause(void);
extern void a_state_resume(void);

extern void a_state_exit(void);

typedef enum {
    A_STATE_STAGE_INVALID,
    A_STATE_STAGE_INIT,
    A_STATE_STAGE_BODY,
    A_STATE_STAGE_FREE,
    A_STATE_STAGE_NUM
} StateStage;

typedef enum {
    A_STATE_BODYSTAGE_INVALID,
    A_STATE_BODYSTAGE_RUN,
    A_STATE_BODYSTAGE_PAUSE,
    A_STATE_BODYSTAGE_NUM
} StateBodyStage;

#define A_STATE(name) void A_STATE__MAKE_NAME(name)(void)
#define A_STATE_INIT if(a_state__stage() == A_STATE_STAGE_INIT)
#define A_STATE_BODY if(a_state__stage() == A_STATE_STAGE_BODY || (a_state__stage() == A_STATE_STAGE_INIT && a_state__setStage(NULL, A_STATE_STAGE_BODY)))
#define A_STATE_FREE if(a_state__stage() == A_STATE_STAGE_FREE)
#define A_STATE_RUN if(a_state__stage() == A_STATE_STAGE_BODY && a_state__bodystage() == A_STATE_BODYSTAGE_RUN)
#define A_STATE_PAUSE if(a_state__stage() == A_STATE_STAGE_BODY && a_state__bodystage() == A_STATE_BODYSTAGE_PAUSE)
#define A_STATE_LOOP while(a_state__unchanged())

extern void a_state_add(const char* name, void* object);
extern void* a_state_get(const char* name);

extern StateStage a_state__stage(void);
extern StateBodyStage a_state__bodystage(void);

extern bool a_state__setStage(StateInstance* state, StateStage stage);
extern void a_state__setBodyStage(StateInstance* state, StateBodyStage bodystage);
extern bool a_state__unchanged(void);
