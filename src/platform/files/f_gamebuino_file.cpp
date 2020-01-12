/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

typedef struct {
    File file;
} FGamebuinoFile;

bool f_platform_api__fileStat(const char* Path, FPathInfo* Info)
{
    File f = SD.open(Path, O_RDONLY);

    if(f) {
        unsigned flags = F_PATH_REAL;
        size_t size = 0;

        if(f.isDirectory()) {
            F_FLAGS_SET(flags, F_PATH_DIR);
        } else {
            F_FLAGS_SET(flags, F_PATH_FILE);
            size = f.size();
        }

        f.close();

        Info->flags = (FPathFlags)flags;
        Info->size = size;

        return true;
    }

    return false;
}

bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size)
{
    File f = SD.open(Path, O_RDONLY);

    if(!f) {
        return false;
    }

    bool ret = f.read(Buffer, Size) == (int)Size;

    f.close();

    return ret;
}

bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size)
{
    File f = SD.open(Path, O_RDWR | O_CREAT);

    if(!f) {
        return false;
    }

    bool ret = f.write(Buffer, Size) == (int)Size;

    f.close();

    return ret;
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

void f_platform_api__filePrint(FPlatformFile* File, const char* String)
{
    F_UNUSED(File);

    SerialUSB.printf(String);
}
#endif // F_CONFIG_SYSTEM_GAMEBUINO
