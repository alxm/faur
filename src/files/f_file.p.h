/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_FILES_FILE_P_H
#define F_INC_FILES_FILE_P_H

#include "general/f_system_includes.h"

typedef struct AFile AFile;

typedef enum {
    F_FILE_READ = F_FLAGS_BIT(0),
    F_FILE_WRITE = F_FLAGS_BIT(1),
    F_FILE_BINARY = F_FLAGS_BIT(2),
} AFileMode;

#include "files/f_path.p.h"

extern AFile* f_file_new(const char* Path, AFileMode Mode);
extern void f_file_free(AFile* File);

extern const APath* f_file_pathGet(const AFile* File);
extern FILE* f_file_handleGet(const AFile* File);

extern uint8_t* f_file_toBuffer(const char* Path);

extern bool f_file_prefixCheck(AFile* File, const char* Prefix);
extern void f_file_prefixWrite(AFile* File, const char* Prefix);

extern bool f_file_read(AFile* File, void* Buffer, size_t Size);
extern bool f_file_write(AFile* File, const void* Buffer, size_t Size);
extern bool f_file_writef(AFile* File, const char* Format, ...);
extern bool f_file_flush(AFile* File);

extern bool f_file_lineRead(AFile* File);
extern const char* f_file_lineBufferGet(const AFile* File);
extern unsigned f_file_lineNumberGet(const AFile* File);

extern bool f_file_rewind(AFile* File);
extern bool f_file_seekStart(AFile* File, int Offset);
extern bool f_file_seekEnd(AFile* File, int Offset);
extern bool f_file_seekCurrent(AFile* File, int Offset);

#endif // F_INC_FILES_FILE_P_H
