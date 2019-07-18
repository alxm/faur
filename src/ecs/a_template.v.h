/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_ECS_TEMPLATE_V_H
#define A_INC_ECS_TEMPLATE_V_H

#include "ecs/a_template.p.h"

typedef struct ATemplate ATemplate;

extern void a_template__init(void);
extern void a_template__uninit(void);

extern const ATemplate* a_template__get(const char* TemplateId, const char* CallerFunction);

extern unsigned a_template__instanceGet(const ATemplate* Template);
extern bool a_template__componentHas(const ATemplate* Template, int ComponentIndex);
extern const void* a_template__dataGet(const ATemplate* Template, int ComponentIndex);

#endif // A_INC_ECS_TEMPLATE_V_H
