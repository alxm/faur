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
bool f_platform_api__fileStat(const char* Path, FPathFlags* Flags)
{
    F_UNUSED(Path);
    F_UNUSED(Flags);

    return false;
}

FPlatformFile* f_platform_api__fileNew(FPath* Path, FFileMode Mode)
{
    F_UNUSED(Path);
    F_UNUSED(Mode);

    return NULL;
}

void f_platform_api__fileFree(FPlatformFile* File)
{
    F_UNUSED(File);

    return;
}

bool f_platform_api__fileRead(FPlatformFile* File, void* Buffer, size_t Size)
{
    F_UNUSED(File);
    F_UNUSED(Buffer);
    F_UNUSED(Size);

    return false;
}

bool f_platform_api__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size)
{
    F_UNUSED(File);
    F_UNUSED(Buffer);
    F_UNUSED(Size);

    return false;
}

void f_platform_api__filePrint(FPlatformFile* File, const char* String)
{
    F_UNUSED(File);
    F_UNUSED(String);

    return;
}
#endif // F_CONFIG_SYSTEM_ODROID_GO
