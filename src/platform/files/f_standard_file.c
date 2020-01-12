/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "f_standard_file.v.h"
#include <faur.v.h>

#if F_CONFIG_FILES_STANDARD
#include <sys/stat.h>

#if F_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

struct FPlatformFile {
    FILE* handle;
};

bool f_platform_api__fileStat(const char* Path, FPathInfo* Info)
{
    struct stat info;

    if(stat(Path, &info) == 0) {
        FPathFlags flags = F_PATH_REAL;
        size_t size = 0;

        if(S_ISREG(info.st_mode)) {
            F_FLAGS_SET(flags, F_PATH_FILE);
            size = (size_t)info.st_size;
        } else if(S_ISDIR(info.st_mode)) {
            F_FLAGS_SET(flags, F_PATH_DIR);
        } else {
            F_FLAGS_SET(flags, F_PATH_OTHER);
        }

        Info->flags = flags;
        Info->size = size;

        return true;
    }

    return false;
}

bool f_platform_api__fileBufferRead(const char* Path, void* Buffer, size_t Size)
{
    struct stat info;

    if(stat(Path, &info) != 0) {
        f_out__error("stat(%s) failed", Path);

        return false;
    }

    FFile* f = f_file_new(Path, F_FILE_READ | F_FILE_BINARY);

    if(f == NULL) {
        return false;
    }

    size_t fileSize = (size_t)info.st_size;
    bool ret = fileSize <= Size && f_file_read(f, Buffer, fileSize);

    f_file_free(f);

    return ret;
}

bool f_platform_api__fileBufferWrite(const char* Path, const void* Buffer, size_t Size)
{
    FFile* f = f_file_new(Path, F_FILE_WRITE | F_FILE_BINARY);

    if(f == NULL) {
        return false;
    }

    bool ret = f_file_write(f, Buffer, Size);

    f_file_free(f);

    return ret;
}

FPlatformFile* f_platform_api__fileNew(FPath* Path, FFileMode Mode)
{
    int index = 0;
    char mode[4];

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_READ)) {
        mode[index++] = 'r';
    } else if(F_FLAGS_TEST_ANY(Mode, F_FILE_WRITE)) {
        mode[index++] = 'w';
    }

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_BINARY)) {
        mode[index++] = 'b';
    }

    mode[index] = '\0';

    FILE* handle = fopen(f_path_getFull(Path), mode);

    if(handle == NULL) {
        return NULL;
    }

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_WRITE)) {
        f_path__flagsSet(Path, F_PATH_FILE | F_PATH_REAL);
    }

    return handle;
}

void f_platform_api__fileFree(FPlatformFile* File)
{
    fclose(File);
}

bool f_platform_api__fileSeek(FPlatformFile* File, int Offset, FFileOffset Origin)
{
    static const int whence[F_FILE__OFFSET_NUM] = {
        [F_FILE__OFFSET_START] = SEEK_SET,
        [F_FILE__OFFSET_CURRENT] = SEEK_CUR,
        [F_FILE__OFFSET_END] = SEEK_END,
    };

    return fseek(File, (long int)Offset, whence[Origin]) == 0;
}

bool f_platform_api__fileRead(FPlatformFile* File, void* Buffer, size_t Size)
{
    return fread(Buffer, Size, 1, File) == 1;
}

bool f_platform_api__fileWrite(FPlatformFile* File, const void* Buffer, size_t Size)
{
    bool ret = fwrite(Buffer, Size, 1, File) == 1;

    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        EM_ASM(
            {
                FS.syncfs(false, function(Error) {});
            },
            0
        );
    #endif

    return ret;
}

bool f_platform_api__fileWritef(FPlatformFile* File, const char* Format, va_list Args)
{
    return vfprintf(File, Format, Args) >= 0;
}

void f_platform_api__filePrint(FPlatformFile* File, const char* String)
{
    fputs(String, File);
}

bool f_platform_api__fileFlush(FPlatformFile* File)
{
    return fflush(File) == 0;
}

int f_platform_api__fileReadChar(FPlatformFile* File)
{
    return fgetc(File);
}

int f_platform_api__fileReadCharUndo(FPlatformFile* File, int Char)
{
    return ungetc(Char, File);
}
#endif // F_CONFIG_FILES_STANDARD
