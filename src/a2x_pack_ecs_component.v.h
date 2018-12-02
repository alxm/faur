/*
    Copyright 2016-2018 Alex Margarit

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

#include "a2x_pack_ecs_component.p.h"

typedef struct {
    size_t size; // total size of AComponentInstance + user data that follows
    AInit* init; // sets component buffer default values
    AFree* free; // does not free the actual component buffer
    size_t dataSize; // size of template data buffer
    AComponentDataInit* dataInit; // init template buffer with info from ABlock
    AFree* dataFree; // does not free the actual template buffer
    AInitWithData* initWithData; // init component buffer with template data
    const char* stringId; // string ID
    unsigned bit; // component's unique bit ID
} AComponent;

typedef struct {
    const AComponent* component; // shared data for all components of same type
    AEntity* entity; // entity this component belongs to
} AComponentInstance;

extern unsigned a_component__tableLen;

extern void a_component__init(unsigned NumComponents);
extern void a_component__uninit(void);

extern int a_component__stringToIndex(const char* StringId);
extern const AComponent* a_component__get(int Component, const char* CallerFunction);

static inline void* a_component__headerGetData(const AComponentInstance* Header)
{
    return (void*)(Header + 1);
}
