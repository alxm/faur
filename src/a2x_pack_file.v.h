/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#pragma once

#include "a2x_pack_file.p.h"

typedef enum {
    A_FILE__OFFSET_INVALID = -1,
    A_FILE__OFFSET_START,
    A_FILE__OFFSET_CURRENT,
    A_FILE__OFFSET_END,
    A_FILE__OFFSET_NUM
} AFileOffset;

typedef struct {
    bool (*seek)(AFile* File, int Offset, AFileOffset Origin);
    bool (*read)(AFile* File, void* Buffer, size_t Size);
    bool (*write)(AFile* File, const void* Buffer, size_t Size);
    bool (*writef)(AFile* File, const char* Format, va_list Args);
    bool (*flush)(AFile* File);
    int (*getchar)(AFile* File);
    int (*ungetchar)(AFile* File, int Char);
} AFileInterface;

#include "a2x_pack_embed.v.h"

struct AFile {
    APath* path;
    const AFileInterface* interface;
    union {
        FILE* handle;
        struct {
            const AEmbeddedFile* data;
            size_t index;
        } e;
    } u;
    char* lineBuffer;
    unsigned lineBufferSize;
    unsigned lineNumber;
    bool eof;
};

extern const AEmbeddedFile* a_file__dataGet(AFile* File);
