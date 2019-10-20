/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#include "f_strbuilder.v.h"
#include <faur.v.h>

struct FStrBuilder {
    char* fmtBuffer;
    size_t fmtSize;
    size_t size;
    size_t index;
    char buffer[];
};

FStrBuilder* f_strbuilder_new(size_t Bytes)
{
    #if F_CONFIG_BUILD_DEBUG
        if(Bytes == 0) {
            F__FATAL("f_strbuilder_new: Invalid size 0");
        }
    #endif

    FStrBuilder* b = f_mem_malloc(sizeof(FStrBuilder) + Bytes);

    b->fmtBuffer = NULL;
    b->fmtSize = 0;
    b->size = Bytes;
    b->index = 0;
    b->buffer[0] = '\0';

    return b;
}

void f_strbuilder_free(FStrBuilder* Builder)
{
    if(Builder == NULL) {
        return;
    }

    f_mem_free(Builder->fmtBuffer);
    f_mem_free(Builder);
}

const char* f_strbuilder_get(FStrBuilder* Builder)
{
    return Builder->buffer;
}

bool f_strbuilder_add(FStrBuilder* Builder, const char* String)
{
    const size_t limit = Builder->size - 1;
    size_t index = Builder->index;

    while(*String != '\0' && index < limit) {
        Builder->buffer[index] = *String;
        String++;
        index++;
    }

    Builder->buffer[index] = '\0';
    Builder->index = index;

    return *String == '\0';
}

bool f_strbuilder_addf(FStrBuilder* Builder, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    va_end(args);

    if(bytesNeeded < 1) {
        return false;
    }

    if(Builder->fmtSize < (size_t)bytesNeeded) {
        f_mem_free(Builder->fmtBuffer);

        Builder->fmtSize = (size_t)bytesNeeded;
        Builder->fmtBuffer = f_mem_malloc(Builder->fmtSize);
    }

    va_start(args, Format);
    vsnprintf(Builder->fmtBuffer, Builder->fmtSize, Format, args);
    va_end(args);

    return f_strbuilder_add(Builder, Builder->fmtBuffer);
}
