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
    #include "f_odroid_go_file.v.h"
}

#include <faur.v.h>

#if F_CONFIG_SYSTEM_ODROID_GO
#include <SD.h>

bool f_platform_api__fileStat(const char* Path, FPathInfo* Info)
{
    File f = SD.open(Path, FILE_READ);

    if(f) {
        unsigned flags = F_PATH_REAL;
        size_t size = 0;

        if(f.isDirectory()) {
            F_FLAGS_SET(flags, F_PATH_DIR);
        } else {
            F_FLAGS_SET(flags, F_PATH_FILE);
            size = f.size();
        }

        Info->flags = flags;
        Info->size = size;

        return true;
    }

    return false;
}

bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size)
{
    File f = SD.open(Path, FILE_READ);

    // f's destructor closes file
    return f && f.read((uint8_t*)Buffer, Size) == Size;
}

bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size)
{
    File f = SD.open(Path, FILE_WRITE);

    // f's destructor closes file
    return f && f.write((const uint8_t*)Buffer, Size) == Size;
}

void f_platform_api__filePrint(FPlatformFile* File, const char* String)
{
    F_UNUSED(File);

    Serial.printf(String);
}
#endif // F_CONFIG_SYSTEM_ODROID_GO
