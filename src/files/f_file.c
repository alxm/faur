/*
    Copyright 2010, 2016-2021 Alex Margarit <alex@alxm.org>
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

#include "f_file.v.h"
#include <faur.v.h>

struct FFile {
    FPath* path;
    union {
        void* file;
        FPlatformFile* platform;
        FFileEmbedded* embedded;
    } f;
    char* lineBuffer;
    unsigned lineBufferSize;
    unsigned lineNumber;
    bool eof;
};

bool f_file_bufferRead(const char* Path, void* Buffer, size_t Size)
{
    bool ret;

    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        ret = f_platform_api__fileBufferRead(Path, Buffer, Size);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        ret = f_file_embedded__bufferRead(Path, Buffer, Size);
    } else {
        ret = false;
    }

    if(!ret) {
        f_out__warning("f_file_bufferRead(%s, %zu): Failed", Path, Size);
    }

    return ret;
}

const void* f_file_bufferReadConst(const char* Path)
{
    const FEmbeddedFile* data = f_embed__fileGet(Path);

    return data ? data->buffer : NULL;
}

bool f_file_bufferWrite(const char* Path, const void* Buffer, size_t Size)
{
    bool ret = f_platform_api__fileBufferWrite(Path, Buffer, Size);

    if(!ret) {
        f_out__error("f_file_bufferWrite(%s, %zu): Failed", Path, Size);
    }

    #if F_CONFIG_FILES_SYNC_AFTER_WRITE
        f_platform_api__fileSync();
    #endif

    return ret;
}

FFile* f_file_new(const char* Path, unsigned Mode)
{
    if(F_FLAGS_TEST_ALL(Mode, F_FILE_READ | F_FILE_WRITE)
        || !F_FLAGS_TEST_ANY(Mode, F_FILE_READ | F_FILE_WRITE)) {

        F__FATAL("f_file_new(%s, %x): Invalid mode", Path, Mode);
    }

    void* file = NULL;
    FPath* path = f_path_new(Path);

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_WRITE)) {
        file = f_platform_api__fileNew(path, Mode);

        if(file) {
            f_path__flagsSet(path, F_PATH_FILE | F_PATH_REAL);
        }
    } else if(f_path_test(path, F_PATH_FILE | F_PATH_REAL)) {
        file = f_platform_api__fileNew(path, Mode);
    } else if(f_path_test(path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        file = f_file_embedded__new(path);
    } else {
        f_out__warning("f_file_new(%s): File does not exist", Path);
    }

    if(file == NULL) {
        f_path_free(path);

        f_out__error("f_file_new(%s, %x): Cannot open", Path, Mode);

        return NULL;
    }

    FFile* f = f_mem_mallocz(sizeof(FFile));

    f->path = path;
    f->f.file = file;

    return f;
}

void f_file_free(FFile* File)
{
    if(File == NULL) {
        return;
    }

    if(f_path_test(File->path, F_PATH_REAL)) {
        f_platform_api__fileFree(File->f.platform);
    } else {
        f_file_embedded__free(File->f.embedded);
    }

    f_path_free(File->path);

    f_mem_free(File->lineBuffer);
    f_mem_free(File);
}

const FPath* f_file_pathGet(const FFile* File)
{
    return File->path;
}

FILE* f_file_handleGet(const FFile* File)
{
    return f_path_test(File->path, F_PATH_REAL)
            ? (FILE*)File->f.platform : NULL;
}

bool f_file_prefixRead(FFile* File, const char* Prefix)
{
    size_t size = strlen(Prefix) + 1;
    char buffer[size];

    f_file_seekStart(File, 0);

    if(!f_file_read(File, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return f_str_equal(buffer, Prefix);
}

void f_file_prefixWrite(FFile* File, const char* Prefix)
{
    f_file_write(File, Prefix, strlen(Prefix) + 1);
}

bool f_file_read(FFile* File, void* Buffer, size_t Size)
{
    bool ret;

    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        ret = f_file_embedded__read(File->f.embedded, Buffer, Size);
    } else {
        ret = f_platform_api__fileRead(File->f.platform, Buffer, Size);
    }

    if(!ret) {
        f_out__warning("f_file_read(%s): Could not read %zu bytes",
                       f_path_getFull(File->path),
                       Size);
    }

    return ret;
}

bool f_file_write(FFile* File, const void* Buffer, size_t Size)
{
    bool ret = f_platform_api__fileWrite(File->f.platform, Buffer, Size);

    if(!ret) {
        f_out__error("f_file_write(%s): Could not write %zu bytes",
                     f_path_getFull(File->path),
                     Size);
    }

    #if F_CONFIG_FILES_SYNC_AFTER_WRITE
        f_platform_api__fileSync();
    #endif

    return ret;
}

bool f_file_writef(FFile* File, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    bool ret = f_platform_api__fileWritef(File->f.platform, Format, args);

    va_end(args);

    if(!ret) {
        f_out__error(
            "f_file_writef(%s): Could not write", f_path_getFull(File->path));
    }

    #if F_CONFIG_FILES_SYNC_AFTER_WRITE
        f_platform_api__fileSync();
    #endif

    return ret;
}

bool f_file_flush(FFile* File)
{
    return f_platform_api__fileFlush(File->f.platform);
}

static int readChar(FFile* File)
{
    int ch;
    bool embedded = f_path_test(File->path, F_PATH_EMBEDDED);

    if(embedded) {
        ch = f_file_embedded__readChar(File->f.embedded);
    } else {
        ch = f_platform_api__fileReadChar(File->f.platform);
    }

    if(ch == '\r') {
        // Check if \r is followed by \n for CRLF line endings
        if(embedded) {
            ch = f_file_embedded__readChar(File->f.embedded);
        } else {
            ch = f_platform_api__fileReadChar(File->f.platform);
        }

        if(ch != '\n') {
            // \r not followed by \n, assume CR line endings and put char back
            if(embedded) {
                ch = f_file_embedded__readCharUndo(File->f.embedded, ch);
            } else {
                ch = f_platform_api__fileReadCharUndo(File->f.platform, ch);
            }

            if(ch != EOF) {
                File->lineNumber++;
                ch = '\r';
            }
        }
    }

    if(ch == '\n') {
        // Sequence was \n or \r\n
        File->lineNumber++;
    } else if(ch == EOF) {
        File->eof = true;
    }

    return ch;
}

bool f_file_lineRead(FFile* File)
{
    int ch;

    do {
        ch = readChar(File);
    } while(ch == '\n' || ch == '\r');

    if(File->eof) {
        return false;
    }

    unsigned index = 0;

    do {
        if(index + 1 >= File->lineBufferSize) {
            unsigned newSize = f_math_maxu(File->lineBufferSize * 2, 64);
            char* newBuffer = f_mem_malloc(newSize);

            if(File->lineBufferSize > 0) {
                memcpy(newBuffer, File->lineBuffer, File->lineBufferSize);
            }

            f_mem_free(File->lineBuffer);

            File->lineBuffer = newBuffer;
            File->lineBufferSize = newSize;
        }

        File->lineBuffer[index++] = (char)ch;
        ch = readChar(File);
    } while(!File->eof && ch != '\n' && ch != '\r');

    File->lineBuffer[index] = '\0';

    return true;
}

const char* f_file_lineBufferGet(const FFile* File)
{
    return File->lineBuffer;
}

unsigned f_file_lineNumberGet(const FFile* File)
{
    return File->lineNumber;
}

bool f_file_rewind(FFile* File)
{
    bool ret;

    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        ret = f_file_embedded__seek(
                File->f.embedded, 0, F_FILE__OFFSET_START);
    } else {
        ret = f_platform_api__fileSeek(
                File->f.platform, 0, F_FILE__OFFSET_START);
    }

    if(ret) {
        File->lineNumber = 0;
        File->eof = false;
    } else {
        f_out__error("f_file_rewind(%s) failed", f_path_getFull(File->path));
    }

    return ret;
}

bool f_file_seekStart(FFile* File, int Offset)
{
    bool ret;

    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        ret = f_file_embedded__seek(
                File->f.embedded, Offset, F_FILE__OFFSET_START);
    } else {
        ret = f_platform_api__fileSeek(
                File->f.platform, Offset, F_FILE__OFFSET_START);
    }

    if(!ret) {
        f_out__error("f_file_seekStart(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool f_file_seekEnd(FFile* File, int Offset)
{
    bool ret;

    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        ret = f_file_embedded__seek(
                File->f.embedded, Offset, F_FILE__OFFSET_END);
    } else {
        ret = f_platform_api__fileSeek(
                File->f.platform, Offset, F_FILE__OFFSET_END);
    }

    if(!ret) {
        f_out__error("f_file_seekEnd(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool f_file_seekCurrent(FFile* File, int Offset)
{
    bool ret;

    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        ret = f_file_embedded__seek(
                File->f.embedded, Offset, F_FILE__OFFSET_CURRENT);
    } else {
        ret = f_platform_api__fileSeek(
                File->f.platform, Offset, F_FILE__OFFSET_CURRENT);
    }

    if(!ret) {
        f_out__error("f_file_seekCurrent(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}
