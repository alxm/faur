/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

typedef struct AFile AFile;

typedef enum {
    A_FILE_READ = A_FLAG_BIT(0),
    A_FILE_WRITE = A_FLAG_BIT(1),
    A_FILE_BINARY = A_FLAG_BIT(2),
} AFileMode;

extern AFile* a_file_new(const char* Path, AFileMode Mode);
extern void a_file_free(AFile* File);

extern bool a_file_prefixCheck(AFile* File, const char* Prefix);
extern void a_file_prefixWrite(AFile* File, const char* Prefix);

extern bool a_file_read(AFile* File, void* Buffer, size_t Size);
extern bool a_file_write(AFile* File, const void* Buffer, size_t Size);
extern bool a_file_writef(AFile* File, char* Format, ...);

extern bool a_file_lineRead(AFile* File);
extern const char* a_file_lineBufferGet(const AFile* File);
extern unsigned a_file_lineNumberGet(const AFile* File);

extern void a_file_rewind(AFile* File);
extern void a_file_seekStart(const AFile* File, long int Offset);
extern void a_file_seekEnd(const AFile* File, long int Offset);
extern void a_file_seekCurrent(const AFile* File, long int Offset);

extern const char* a_file_pathGet(const AFile* File);
extern const char* a_file_nameGet(const AFile* File);
extern FILE* a_file_handleGet(const AFile* File);

extern uint8_t* a_file_toBuffer(const char* Path);
