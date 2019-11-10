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

#include "../general/f_system_includes.h"

typedef struct FFile FFile;

typedef enum {
    F_FILE_READ = F_FLAGS_BIT(0),
    F_FILE_WRITE = F_FLAGS_BIT(1),
    F_FILE_BINARY = F_FLAGS_BIT(2),
} FFileMode;

#include "../files/f_path.p.h"

extern FFile* f_file_new(const char* Path, FFileMode Mode);
extern void f_file_free(FFile* File);

extern const FPath* f_file_pathGet(const FFile* File);
extern FILE* f_file_handleGet(const FFile* File);

extern uint8_t* f_file_toBuffer(const char* Path);

extern bool f_file_prefixCheck(FFile* File, const char* Prefix);
extern void f_file_prefixWrite(FFile* File, const char* Prefix);

extern bool f_file_read(FFile* File, void* Buffer, size_t Size);
extern bool f_file_write(FFile* File, const void* Buffer, size_t Size);
extern bool f_file_writef(FFile* File, const char* Format, ...);
extern bool f_file_flush(FFile* File);

extern bool f_file_lineRead(FFile* File);
extern const char* f_file_lineBufferGet(const FFile* File);
extern unsigned f_file_lineNumberGet(const FFile* File);

extern bool f_file_rewind(FFile* File);
extern bool f_file_seekStart(FFile* File, int Offset);
extern bool f_file_seekEnd(FFile* File, int Offset);
extern bool f_file_seekCurrent(FFile* File, int Offset);

#endif // F_INC_FILES_FILE_P_H
