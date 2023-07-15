/*
    Copyright 2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_ECS_P_H
#define F_INC_ECS_ECS_P_H

#include "../general/f_system_includes.h"

#include "../ecs/f_component.p.h"
#include "../ecs/f_system.p.h"

#define F_ECS(Components, Systems) \
    static const FComponent* const f__c[] = Components; \
    static const FSystem* const f__s[] = Systems; \
    f__ecs_init(f__c, F_ARRAY_LEN(f__c), f__s, F_ARRAY_LEN(f__s))

#define F_COMPONENTS(...) {__VA_ARGS__}
#define F_SYSTEMS(...) {__VA_ARGS__}

extern void f__ecs_init(const FComponent* const Components[], size_t ComponentsNum, const FSystem* const Systems[], size_t SystemsNum);

#endif // F_INC_ECS_ECS_P_H
