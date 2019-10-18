/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_ENTITY_P_H
#define F_INC_ECS_ENTITY_P_H

#include "general/f_system_includes.h"

typedef struct AEntity AEntity;

extern AEntity* f_entity_new(const char* Template, const void* Context);

extern void f_entity_debugSet(AEntity* Entity, bool DebugOn);

extern const char* f_entity_idGet(const AEntity* Entity);

extern AEntity* f_entity_parentGet(const AEntity* Entity);
extern void f_entity_parentSet(AEntity* Entity, AEntity* Parent);
extern bool f_entity_parentHas(const AEntity* Child, const AEntity* PotentialParent);

extern void f_entity_refInc(AEntity* Entity);
extern void f_entity_refDec(AEntity* Entity);

extern bool f_entity_removedGet(const AEntity* Entity);
extern void f_entity_removedSet(AEntity* Entity);

extern bool f_entity_activeGet(const AEntity* Entity);
extern void f_entity_activeSet(AEntity* Entity);
extern void f_entity_activeSetRemove(AEntity* Entity);
extern void f_entity_activeSetPermanent(AEntity* Entity);

extern void* f_entity_componentAdd(AEntity* Entity, int ComponentIndex);
extern bool f_entity_componentHas(const AEntity* Entity, int ComponentIndex);
extern void* f_entity_componentGet(const AEntity* Entity, int ComponentIndex);
extern void* f_entity_componentReq(const AEntity* Entity, int ComponentIndex);

extern bool f_entity_muteGet(const AEntity* Entity);
extern void f_entity_muteInc(AEntity* Entity);
extern void f_entity_muteDec(AEntity* Entity);

#endif // F_INC_ECS_ENTITY_P_H
