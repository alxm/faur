/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_file_real.v.h"
#include <a2x.v.h>
#include <sys/stat.h>

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #include <emscripten.h>
#endif

static bool fileSeek(AFile* File, int Offset, AFileOffset Origin)
{
    static const int whence[A_FILE__OFFSET_NUM] = {
        [A_FILE__OFFSET_START] = SEEK_SET,
        [A_FILE__OFFSET_CURRENT] = SEEK_CUR,
        [A_FILE__OFFSET_END] = SEEK_END,
    };

    return fseek(File->u.handle, (long int)Offset, whence[Origin]) == 0;
}

static bool fileRead(AFile* File, void* Buffer, size_t Size)
{
    return fread(Buffer, Size, 1, File->u.handle) == 1;
}

static bool fileWrite(AFile* File, const void* Buffer, size_t Size)
{
    bool ret = fwrite(Buffer, Size, 1, File->u.handle) == 1;

    #if A_CONFIG_SYSTEM_EMSCRIPTEN
        EM_ASM(
            {
                FS.syncfs(false, function(Error) {});
            },
            0
        );
    #endif

    return ret;
}

static bool fileWritef(AFile* File, const char* Format, va_list Args)
{
    return vfprintf(File->u.handle, Format, Args) >= 0;
}

static bool fileFlush(AFile* File)
{
    return fflush(File->u.handle) == 0;
}

static int fileGetChar(AFile* File)
{
    return fgetc(File->u.handle);
}

static int fileUnGetChar(AFile* File, int Char)
{
    return ungetc(Char, File->u.handle);
}

static const AFileInterface g_interface = {
    .seek = fileSeek,
    .read = fileRead,
    .write = fileWrite,
    .writef = fileWritef,
    .flush = fileFlush,
    .getchar = fileGetChar,
    .ungetchar = fileUnGetChar,
};

AFile* a_file_real__new(APath* Path, AFileMode Mode)
{
    int index = 0;
    char mode[4];

    if(A_FLAGS_TEST_ANY(Mode, A_FILE_READ)) {
        mode[index++] = 'r';
    } else if(A_FLAGS_TEST_ANY(Mode, A_FILE_WRITE)) {
        mode[index++] = 'w';
    }

    if(A_FLAGS_TEST_ANY(Mode, A_FILE_BINARY)) {
        mode[index++] = 'b';
    }

    mode[index] = '\0';

    FILE* handle = fopen(a_path_getFull(Path), mode);

    if(handle == NULL) {
        a_out__error("a_file_new(%s): Cannot open for '%s'",
                     a_path_getFull(Path),
                     mode);

        return NULL;
    }

    if(A_FLAGS_TEST_ANY(Mode, A_FILE_WRITE)) {
        a_path__flagsSet(Path, A_PATH_FILE | A_PATH_REAL);
    }

    AFile* f = a_mem_zalloc(sizeof(AFile));

    f->path = Path;
    f->interface = &g_interface;
    f->u.handle = handle;

    return f;
}

uint8_t* a_file_real__toBuffer(const char* Path)
{
    struct stat info;

    if(stat(Path, &info) != 0) {
        a_out__error("a_file_toBuffer: stat(%s) failed", Path);
        return NULL;
    }

    AFile* f = a_file_new(Path, A_FILE_READ | A_FILE_BINARY);

    if(f == NULL) {
        return NULL;
    }

    size_t size = (size_t)info.st_size;
    uint8_t* buffer = a_mem_malloc(size);

    if(!a_file_read(f, buffer, size)) {
        a_mem_free(buffer);
        buffer = NULL;
    }

    a_file_free(f);

    return buffer;
}
