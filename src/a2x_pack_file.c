/*
    Copyright 2010, 2016-2018 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_file.v.h"

#include <sys/stat.h>

#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

struct AFile {
    FILE* handle;
    char* path;
    char* name;
    char* lineBuffer;
    unsigned lineBufferSize;
    unsigned lineNumber;
    bool eof;
};

AFile* a_file_new(const char* Path, AFileMode Mode)
{
    int index = 0;
    char mode[4];

    if(Mode & A_FILE_READ) {
        mode[index++] = 'r';
    } else if(Mode & A_FILE_WRITE) {
        mode[index++] = 'w';
    }

    if(Mode & A_FILE_BINARY) {
        mode[index++] = 'b';
    }

    mode[index] = '\0';

    FILE* handle = fopen(Path, mode);

    if(handle == NULL) {
        a_out__error("a_file_new: Can't open %s for '%s'", Path, mode);
        return NULL;
    }

    AFile* f = a_mem_malloc(sizeof(AFile));

    f->handle = handle;
    f->path = a_str_prefixGetToLast(Path, '/');
    f->name = a_str_suffixGetFromLast(Path, '/');
    f->lineBuffer = NULL;
    f->lineBufferSize = 0;
    f->lineNumber = 0;
    f->eof = false;

    if(f->name == NULL) {
        f->name = a_str_dup(Path);
    }

    if(f->path == NULL) {
        f->path = a_str_dup(".");
    }

    return f;
}

void a_file_free(AFile* File)
{
    if(File == NULL) {
        return;
    }

    free(File->path);
    free(File->name);
    free(File->lineBuffer);

    if(File->handle) {
        fclose(File->handle);
    }

    free(File);
}

bool a_file_prefixCheck(AFile* File, const char* Prefix)
{
    size_t size = strlen(Prefix) + 1;
    char buffer[size];

    fseek(File->handle, 0, SEEK_SET);

    if(!a_file_read(File, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return a_str_equal(buffer, Prefix);
}

void a_file_prefixWrite(AFile* File, const char* Prefix)
{
    a_file_write(File, Prefix, strlen(Prefix) + 1);
}

bool a_file_read(AFile* File, void* Buffer, size_t Size)
{
    size_t readCount;

    readCount = fread(Buffer, Size, 1, File->handle);

    if(readCount != 1) {
        a_out__warning(
            "a_file_read: Could not read %u bytes from %s", Size, File->name);
        return false;
    }

    return true;
}

bool a_file_write(AFile* File, const void* Buffer, size_t Size)
{
    size_t writeCount;

    writeCount = fwrite(Buffer, Size, 1, File->handle);

    if(writeCount != 1) {
        a_out__error(
            "a_file_write: Could not write %u bytes to %s", Size, File->name);
        return false;
    }

    return true;
}

bool a_file_writef(AFile* File, char* Format, ...)
{
    int ret;
    va_list args;
    va_start(args, Format);

    ret = vfprintf(File->handle, Format, args);

    va_end(args);

    if(ret < 0) {
        a_out__error("a_file_writef: Could not write to %s", File->name);
        return false;
    }

    return true;
}

static int readChar(AFile* File)
{
    int ch = fgetc(File->handle);

    if(ch == '\r') {
        // Check if \r is followed by \n for CRLF line endings
        ch = fgetc(File->handle);

        if(ch != '\n') {
            // \r not followed by \n, assume CR line endings and put char back
            ch = ungetc(ch, File->handle);

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

bool a_file_lineRead(AFile* File)
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
            unsigned newSize = a_math_maxu(File->lineBufferSize * 2, 64);
            char* newBuffer = a_mem_malloc(newSize);

            if(File->lineBufferSize > 0) {
                memcpy(newBuffer, File->lineBuffer, File->lineBufferSize);
            }

            free(File->lineBuffer);

            File->lineBuffer = newBuffer;
            File->lineBufferSize = newSize;
        }

        File->lineBuffer[index++] = (char)ch;
        ch = readChar(File);
    } while(!File->eof && ch != '\n' && ch != '\r');

    File->lineBuffer[index] = '\0';

    return true;
}

const char* a_file_lineBufferGet(const AFile* File)
{
    return File->lineBuffer;
}

unsigned a_file_lineNumberGet(const AFile* File)
{
    return File->lineNumber;
}

void a_file_rewind(AFile* File)
{
    rewind(File->handle);
    File->lineNumber = 0;
}

void a_file_seekStart(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_SET) != 0) {
        a_out__error("a_file_seekStart(%s): fseek(%ld) failed",
                     File->name,
                     Offset);
    }
}

void a_file_seekEnd(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_END) != 0) {
        a_out__error("a_file_seekEnd(%s): fseek(%ld) failed",
                     File->name,
                     Offset);
    }
}

void a_file_seekCurrent(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_CUR) != 0) {
        a_out__error("a_file_seekCurrent(%s): fseek(%ld) failed",
                     File->name,
                     Offset);
    }
}

const char* a_file_pathGet(const AFile* File)
{
    return File->path;
}

const char* a_file_nameGet(const AFile* File)
{
    return File->name;
}

FILE* a_file_handleGet(const AFile* File)
{
    return File->handle;
}

bool a_file_exists(const char* Path)
{
    return access(Path, F_OK) == 0;
}

uint8_t* a_file_toBuffer(const char* Path)
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
        free(buffer);
        buffer = NULL;
    }

    a_file_free(f);

    return buffer;
}
