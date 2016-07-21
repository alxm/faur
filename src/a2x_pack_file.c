/*
    Copyright 2010 Alex Margarit

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

struct AFile {
    FILE* handle;
    char* modes;
    char* path;
    char* name;
    char* line;
    int eof;
};

AFile* a_file_open(const char* path, const char* modes)
{
    FILE* const handle = fopen(path, modes);

    if(!handle) {
        a_out__error("a_file_open: Can't open %s for '%s'", path, modes);
        return NULL;
    }

    AFile* const f = a_mem_malloc(sizeof(AFile));

    f->handle = handle;
    f->modes = a_str_dup(modes);
    f->path = a_str_getPrefixLastFind(path, '/');
    f->name = a_str_getSuffixLastFind(path, '/');
    f->line = NULL;
    f->eof = 0;

    return f;
}

void a_file_close(AFile* f)
{
    free(f->modes);
    free(f->path);
    free(f->name);
    free(f->line);

    if(f->handle) {
        fclose(f->handle);
    }

    free(f);
}

bool a_file_checkPrefix(AFile* f, const char* prefix)
{
    const size_t size = strlen(prefix) + 1;
    char buffer[size];

    fseek(f->handle, 0, SEEK_SET);

    if(!a_file_read(f, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return a_str_equal(buffer, prefix);
}

void a_file_writePrefix(AFile* f, const char* prefix)
{
    a_file_write(f, prefix, strlen(prefix) + 1);
}

bool a_file_read(AFile* f, void* buffer, size_t size)
{
    size_t readCount;

    readCount = fread(buffer, size, 1, f->handle);

    if(readCount != 1) {
        a_out__warning("a_file_read: could not read %u bytes from %s",
                       size, f->name);
        return false;
    }

    return true;
}

bool a_file_write(AFile* f, const void* buffer, size_t size)
{
    size_t writeCount;

    writeCount = fwrite(buffer, size, 1, f->handle);

    if(writeCount != 1) {
        a_out__error("a_file_write: could not write %u bytes to %s",
                     size, f->name);
        return false;
    }

    return true;
}

bool a_file_writef(AFile* f, char* fmt, ...)
{
    int ret;
    va_list args;
    va_start(args, fmt);

    ret = vfprintf(f->handle, fmt, args);

    va_end(args);

    if(ret < 0) {
        a_out__error("a_file_writef: could not write to %s", f->name);
        return false;
    }

    return true;
}

bool a_file_readLine(AFile* f)
{
    free(f->line);
    f->line = NULL;

    if(f->eof) {
        return false;
    }

    int offset = 1;
    FILE* const handle = f->handle;

    while(offset == 1 && !f->eof) {
        int c;

        for(c = fgetc(handle); !iscntrl(c) && c != EOF; c = fgetc(handle)) {
            offset++;
        }

        f->eof = c == EOF;
    }

    if(offset > 1) {
        if(f->eof) {
            rewind(handle);
            fseek(handle, -(offset - 1), SEEK_END);
        } else {
            fseek(handle, -offset, SEEK_CUR);
        }

        char* const str = a_mem_malloc(offset * sizeof(char));

        for(int i = 0; i < offset - 1; i++) {
            str[i] = fgetc(handle);
        }

        str[offset - 1] = '\0';
        f->line = str;

        fseek(handle, 1, SEEK_CUR);

        return true;
    }

    return false;
}

char* a_file_getLine(const AFile* f)
{
    return f->line;
}

const char* a_file_path(const AFile* f)
{
    return f->path;
}

const char* a_file_name(const AFile* f)
{
    return f->name;
}

FILE* a_file_handle(const AFile* f)
{
    return f->handle;
}

bool a_file_exists(const char* path)
{
    FILE* const f = fopen(path, "r");

    if(f) {
        fclose(f);
        return true;
    }

    return false;
}

bool a_file_isDir(const char* f)
{
    struct stat info;
    stat(f, &info);

    return S_ISDIR(info.st_mode);
}

size_t a_file_size(const char* f)
{
    struct stat info;
    stat(f, &info);

    return info.st_size;
}

uint8_t* a_file_toBuffer(const char* path)
{
    AFile* f = a_file_open(path, "r");

    if(!f) {
        return NULL;
    }

    const size_t size = a_file_size(path);
    uint8_t* buffer = a_mem_malloc(size);

    if(!a_file_read(f, buffer, size)) {
        free(buffer);
        buffer = NULL;
    }

    a_file_close(f);

    return buffer;
}
