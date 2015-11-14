/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_strpool.v.h"

struct StringPool {
    char* data;
    unsigned int size;
    unsigned int offset;
};

StringPool* a_strpool_new(unsigned int bytes)
{
    StringPool* const sp = malloc(sizeof(StringPool));

    sp->data = malloc(bytes);
    sp->size = bytes;
    sp->offset = 0;

    return sp;
}

void a_strpool_free(StringPool* sp)
{
    free(sp->data);
    free(sp);
}

char* a_strpool_alloc(StringPool* sp, unsigned int size)
{
    const unsigned int old_offset = sp->offset;
    const unsigned int new_offset = old_offset + size;

    if(new_offset > sp->size) {
        return NULL;
    }

    sp->offset = new_offset;

    return sp->data + old_offset;
}
