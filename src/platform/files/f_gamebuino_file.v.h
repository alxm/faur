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

#ifndef F_INC_PLATFORM_FILES_GAMEBUINO_FILE_V_H
#define F_INC_PLATFORM_FILES_GAMEBUINO_FILE_V_H

#include "f_gamebuino_file.p.h"

#include "../../files/f_file.v.h"
#include "../../files/f_path.v.h"

extern bool f_platform_api_gamebuino__fileStat(const char* Path, FPathInfo* Info);

extern bool f_platform_api_gamebuino__fileBufferRead(const char* Path, void* Buffer, size_t Size);
extern bool f_platform_api_gamebuino__fileBufferWrite(const char* Path, const void* Buffer, size_t Size);

extern FPlatformFile* f_platform_api_gamebuino__fileNew(const FPath* Path, unsigned Mode);
extern void f_platform_api_gamebuino__fileFree(FPlatformFile* File);

extern bool f_platform_api_gamebuino__fileRead(FPlatformFile* File, void* Buffer, size_t Size);
extern bool f_platform_api_gamebuino__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size);

extern void f_platform_api_gamebuino__filePrint(FPlatformFile* File, const char* String);

#endif // F_INC_PLATFORM_FILES_GAMEBUINO_FILE_V_H
