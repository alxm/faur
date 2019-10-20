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

#ifndef F_INC_ECS_ECS_V_H
#define F_INC_ECS_ECS_V_H

#include "ecs/f_ecs.p.h"

typedef enum {
    F_ECS__INVALID = -1,
    F_ECS__DEFAULT, // no pending changes
    F_ECS__NEW, // new entities that aren't in any systems yet
    F_ECS__RESTORE, // entities matched to systems, to be added to them
    F_ECS__FLUSH, // muted or removed entities, to be flushed from systems
    F_ECS__FREE, // entities to be freed at the end of current frame
    F_ECS__NUM
} FEcsListId;

#include "data/f_list.v.h"
#include "general/f_main.v.h"

extern const FPack f_pack__ecs;

extern FList* f_ecs__listGet(FEcsListId List);
extern unsigned f_ecs__listGetSum(void);

extern bool f_ecs__refDecIgnoreGet(void);
extern void f_ecs__refDecIgnoreSet(bool IgnoreRefDec);

extern void f_ecs__tick(void);

extern void f_ecs__flushEntitiesFromSystems(void);

#endif // F_INC_ECS_ECS_V_H
