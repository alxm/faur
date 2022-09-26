/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_FILES_STANDARD_FILE_V_H
#define F_INC_PLATFORM_FILES_STANDARD_FILE_V_H

#include "f_standard_file.p.h"

#include "../../files/f_file.v.h"
#include "../../files/f_path.v.h"

extern bool f_platform_api_std__dirCreate(const char* Path);

extern bool f_platform_api_std__fileStat(const char* Path, FPathInfo* Info);

extern bool f_platform_api_std__fileBufferRead(const char* Path, void* Buffer, size_t Size);
extern bool f_platform_api_std__fileBufferWrite(const char* Path, const void* Buffer, size_t Size);

extern FPlatformFile* f_platform_api_std__fileNew(const FPath* Path, unsigned Mode);
extern void f_platform_api_std__fileFree(FPlatformFile* File);

extern bool f_platform_api_std__fileSeek(FPlatformFile* File, int Offset, FFileOffset Origin);
extern bool f_platform_api_std__fileRead(FPlatformFile* File, void* Buffer, size_t Size);
extern bool f_platform_api_std__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size);
extern bool f_platform_api_std__fileWritef(FPlatformFile* File, const char* Format, va_list Args);

extern void f_platform_api_std__filePrint(FPlatformFile* File, const char* String);

extern bool f_platform_api_std__fileFlush(FPlatformFile* File);

extern int f_platform_api_std__fileReadChar(FPlatformFile* File);
extern int f_platform_api_std__fileReadCharUndo(FPlatformFile* File, int Char);

#endif // F_INC_PLATFORM_FILES_STANDARD_FILE_V_H
