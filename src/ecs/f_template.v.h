/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

extern void f_template__init(void);
extern void f_template__uninit(void);

extern const FTemplate* f_template__get(const char* TemplateId);

extern void f_template__initRun(const FTemplate* Template, FEntity* Entity, const void* Context);
extern unsigned f_template__instanceGet(const FTemplate* Template);
extern bool f_template__componentHas(const FTemplate* Template, int ComponentIndex);
extern const void* f_template__dataGet(const FTemplate* Template, int ComponentIndex);

#endif // F_INC_ECS_TEMPLATE_V_H