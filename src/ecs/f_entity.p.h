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

#include "../general/f_system_includes.h"

typedef struct FEntity FEntity;

extern FEntity* f_entity_new(const char* Template, const void* Context);

extern void f_entity_debugSet(FEntity* Entity, bool DebugOn);

extern const char* f_entity_idGet(const FEntity* Entity);

extern FEntity* f_entity_parentGet(const FEntity* Entity);
extern void f_entity_parentSet(FEntity* Entity, FEntity* Parent);
extern bool f_entity_parentHas(const FEntity* Child, const FEntity* PotentialParent);

extern void f_entity_refInc(FEntity* Entity);
extern void f_entity_refDec(FEntity* Entity);

extern bool f_entity_removedGet(const FEntity* Entity);
extern void f_entity_removedSet(FEntity* Entity);

extern bool f_entity_activeGet(const FEntity* Entity);
extern void f_entity_activeSet(FEntity* Entity);
extern void f_entity_activeSetRemove(FEntity* Entity);
extern void f_entity_activeSetPermanent(FEntity* Entity);

extern void* f_entity_componentAdd(FEntity* Entity, unsigned ComponentIndex);
extern bool f_entity_componentHas(const FEntity* Entity, unsigned ComponentIndex);
extern void* f_entity_componentGet(const FEntity* Entity, unsigned ComponentIndex);
extern void* f_entity_componentReq(const FEntity* Entity, unsigned ComponentIndex);

extern bool f_entity_muteGet(const FEntity* Entity);
extern void f_entity_muteInc(FEntity* Entity);
extern void f_entity_muteDec(FEntity* Entity);

#endif // F_INC_ECS_ENTITY_P_H
