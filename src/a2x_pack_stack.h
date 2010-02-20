/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_STACK_H
#define A2X_PACK_STACK_H

#include "a2x_pack_list.h"

typedef List Stack;

#define a_stack_set()           a_list_set()
#define a_stack_free(s)         a_list_free(s)
#define a_stack_freeContent(s)  a_list_freeContent(s)
#define a_stack_empty(s)        a_list_empty(s)
#define a_stack_emptyContent(s) a_list_emptyContent(s)
#define a_stack_push(s, v)      a_list_addFirst(s, v)
#define a_stack_pop(s)          a_list_pop(s)
#define a_stack_peek(s)         a_list_peek(s)
#define a_stack_isEmpty(s)      a_list_isEmpty(s)

#endif // A2X_PACK_STACK_H
