/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#include <a2x.v.h>

struct AStrBuilder {
    char* fmtBuffer;
    size_t fmtSize;
    size_t size;
    size_t index;
    char buffer[];
};

AStrBuilder* a_strbuilder_new(size_t Bytes)
{
    if(Bytes == 0) {
        A__FATAL("a_strbuilder_new: Invalid size 0");
    }

    AStrBuilder* b = a_mem_malloc(sizeof(AStrBuilder) + Bytes);

    b->fmtBuffer = NULL;
    b->fmtSize = 0;
    b->size = Bytes;
    b->index = 0;
    b->buffer[0] = '\0';

    return b;
}

void a_strbuilder_free(AStrBuilder* Builder)
{
    if(Builder == NULL) {
        return;
    }

    free(Builder->fmtBuffer);
    free(Builder);
}

const char* a_strbuilder_get(AStrBuilder* Builder)
{
    return Builder->buffer;
}

bool a_strbuilder_add(AStrBuilder* Builder, const char* String)
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

bool a_strbuilder_addf(AStrBuilder* Builder, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    va_end(args);

    if(bytesNeeded < 1) {
        return false;
    }

    if(Builder->fmtSize < (size_t)bytesNeeded) {
        free(Builder->fmtBuffer);

        Builder->fmtSize = (size_t)bytesNeeded;
        Builder->fmtBuffer = a_mem_malloc(Builder->fmtSize);
    }

    va_start(args, Format);
    vsnprintf(Builder->fmtBuffer, Builder->fmtSize, Format, args);
    va_end(args);

    return a_strbuilder_add(Builder, Builder->fmtBuffer);
}
