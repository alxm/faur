/*
    Copyright 2016 Alex Margarit

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

#include "a2x_pack_entity.v.h"

void a_entity__init(void)
{
    //
}

void a_entity__uninit(void)
{
    //
}

void a_component_new(const char* Name, size_t Size, AComponentTick Tick, AComponentDraw Draw)
{
    //
}

AEntity* a_component_getEntity(void* Component)
{
    return NULL;
}

AEntity* a_entity_new(void)
{
    return NULL;
}

void a_entity_free(AEntity* Entity)
{
    //
}

void a_entity_addComponent(AEntity* Entity, const char* Component)
{
    //
}

void* a_entity_getComponent(AEntity* Entity, const char* Component)
{
    return NULL;
}
