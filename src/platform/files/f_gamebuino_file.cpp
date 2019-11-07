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

extern "C" {
    #include "f_gamebuino_file.v.h"
}

#include <faur.v.h>

#if F_CONFIG_FILES_LIB_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

typedef struct {
    File file;
} FGamebuinoFile;

bool f_platform_api__fileStat(const char* Path, FPathFlags* Flags)
{
    File file = SD.open(Path, O_RDONLY);

    if(file) {
        file.close();

        *Flags = (FPathFlags)(F_PATH_REAL | F_PATH_FILE);

        return true;
    }

    return false;
}

FPlatformFile* f_platform_api__fileNew(FPath* Path, FFileMode Mode)
{
    oflag_t flags = 0;

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_READ)) {
        flags |= O_RDONLY;
    } else if(F_FLAGS_TEST_ANY(Mode, F_FILE_WRITE)) {
        flags |= O_RDWR | O_CREAT;
    }

    File file = SD.open(f_path_getFull(Path), flags);

    if(!file) {
        return NULL;
    }

    FGamebuinoFile* f = (FGamebuinoFile*)f_mem_malloc(sizeof(FGamebuinoFile));

    f->file = file;

    return f;
}

void f_platform_api__fileFree(FPlatformFile* File)
{
    if(File == NULL) {
        return;
    }

    FGamebuinoFile* f = (FGamebuinoFile*)File;

    f->file.close();

    f_mem_free(File);
}

bool f_platform_api__fileRead(FPlatformFile* File, void* Buffer, size_t Size)
{
    FGamebuinoFile* f = (FGamebuinoFile*)File;

    return f->file.read(Buffer, Size) == (int)Size;
}

bool f_platform_api__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size)
{
    FGamebuinoFile* f = (FGamebuinoFile*)File;

    return f->file.write(Buffer, Size) == (int)Size;
}
#endif // F_CONFIG_FILES_LIB_GAMEBUINO
