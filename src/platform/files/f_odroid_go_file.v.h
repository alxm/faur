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

#ifndef F_INC_PLATFORM_FILES_ODROID_GO_FILE_V_H
#define F_INC_PLATFORM_FILES_ODROID_GO_FILE_V_H

#include "f_odroid_go_file.p.h"

#include "../../files/f_file.v.h"
#include "../../files/f_path.v.h"

extern bool f_platform_api_odroidgo__fileStat(const char* Path, FPathInfo* Info);

extern bool f_platform_api_odroidgo__fileBufferRead(const char* Path, void* Buffer, size_t Size);
extern bool f_platform_api_odroidgo__fileBufferWrite(const char* Path, const void* Buffer, size_t Size);

extern void f_platform_api_odroidgo__filePrint(FPlatformFile* File, const char* String);

#endif // F_INC_PLATFORM_FILES_ODROID_GO_FILE_V_H
