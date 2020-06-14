/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_TEMPLATE_V_H
#define F_INC_ECS_TEMPLATE_V_H

#include "f_template.p.h"

typedef struct FTemplate FTemplate;

#include "../data/f_bitfield.v.h"

struct FTemplate {
    const FTemplate* parent; // Template chain
    FEntityInit* init; // Optional, runs after components init and parent init
    FList* componentsOwn; // FList<const FComponent*> this template only
    FList* componentsAll; // FList<const FComponent*> this template or parent
    FBitfield* componentsBits; // Set if this template or parent has component
    const FBlock* block; // Only valid while reading current config file
    uint32_t iNumber; // Incremented by every new entity
    void* data[1]; // [f_component__num] Loaded config data, or NULL
                   // Component bit might be set even if data is NULL
};

extern void f_template__init(void);
extern void f_template__uninit(void);

extern const FTemplate* f_template__get(const char* Id);

extern void f_template__initRun(const FTemplate* Template, FEntity* Entity, const void* Context);

#endif // F_INC_ECS_TEMPLATE_V_H
