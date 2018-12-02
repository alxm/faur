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

#include "a2x_system_includes.h"

#include "a2x_pack_block.p.h"
#include "a2x_pack_ecs_entity.p.h"

typedef void AComponentDataInit(void* Data, const ABlock* Block, const void* Context);
typedef void AInitWithData(void* Self, const void* Data, const void* Context);

extern void a_component_new(int Index, const char* StringId, size_t Size, AInit* Init, AFree* Free);

extern const void* a_component_dataGet(const void* Component);
extern void a_component_dataSet(int Index, size_t Size, AComponentDataInit* Init, AFree* Free, AInitWithData* InitWithData);

extern AEntity* a_component_entityGet(const void* Component);
