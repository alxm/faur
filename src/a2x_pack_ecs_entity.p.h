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

typedef struct AEntity AEntity;
typedef void AMessageHandler(AEntity* To, AEntity* From);

extern AEntity* a_entity_new(const char* Id, void* Context);

extern void a_entity_debugSet(AEntity* Entity, bool DebugOn);

extern const char* a_entity_idGet(const AEntity* Entity);
extern void* a_entity_contextGet(const AEntity* Entity);

extern AEntity* a_entity_parentGet(const AEntity* Entity);
extern void a_entity_parentSet(AEntity* Entity, AEntity* Parent);

extern void a_entity_refInc(AEntity* Entity);
extern void a_entity_refDec(AEntity* Entity);

extern bool a_entity_removeGet(const AEntity* Entity);
extern void a_entity_removeSet(AEntity* Entity);

extern bool a_entity_activeGet(const AEntity* Entity);
extern void a_entity_activeSet(AEntity* Entity);
extern void a_entity_activeSetPermanent(AEntity* Entity);

extern void* a_entity_componentAdd(AEntity* Entity, int Component);
extern bool a_entity_componentHas(const AEntity* Entity, int Component);
extern void* a_entity_componentGet(const AEntity* Entity, int Component);
extern void* a_entity_componentReq(const AEntity* Entity, int Component);

extern bool a_entity_muteGet(const AEntity* Entity);
extern void a_entity_muteSet(AEntity* Entity, bool DoMute);

extern void a_entity_messageHandlerSet(AEntity* Entity, const char* Message, AMessageHandler* Handler);
extern void a_entity_messageSend(AEntity* To, AEntity* From, const char* Message);
