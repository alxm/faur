/*
    Copyright 2016, 2017 Alex Margarit

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

typedef struct AEntity AEntity;
typedef void AMessageHandler(AEntity* To, AEntity* From);
typedef void ASystemHandler(AEntity* Entity);
typedef int ASystemSort(AEntity* A, AEntity* B);

extern void a_component_declare(const char* Name, size_t Size, AFree* Free);
extern AEntity* a_component_getEntity(const void* Component);

extern AEntity* a_entity_new(const char* Id, void* Context);
extern const char* a_entity_getId(const AEntity* Entity);
extern void* a_entity_getContext(const AEntity* Entity);
extern AEntity* a_entity_getParent(const AEntity* Entity);
extern void a_entity_setParent(AEntity* Entity, AEntity* Parent);
extern void a_entity_reference(AEntity* Entity);
extern void a_entity_release(AEntity* Entity);
extern void a_entity_remove(AEntity* Entity);
extern bool a_entity_isRemoved(const AEntity* Entity);
extern void a_entity_markActive(AEntity* Entity);
extern bool a_entity_isActive(const AEntity* Entity);
extern void* a_entity_addComponent(AEntity* Entity, const char* Component);
extern bool a_entity_hasComponent(const AEntity* Entity, const char* Component);
extern void* a_entity_getComponent(const AEntity* Entity, const char* Component);
extern void* a_entity_requireComponent(const AEntity* Entity, const char* Component);
extern void a_entity_mute(AEntity* Entity);
extern void a_entity_unmute(AEntity* Entity);
extern bool a_entity_isMuted(const AEntity* Entity);
extern void a_entity_setMessageHandler(AEntity* Entity, const char* Message, AMessageHandler* Handler, bool HandleImmediately);
extern void a_entity_sendMessage(AEntity* To, AEntity* From, const char* Message);

extern void a_system_declare(const char* Name, const char* Components, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities);
extern void a_system_execute(const char* Systems);
extern void a_system_flushNewEntities(void);
extern void a_system_mute(const char* Systems);
extern void a_system_unmute(const char* Systems);
