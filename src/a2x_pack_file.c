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

struct File {
    FILE* handle;
    char* modes;
    char* path;
    char* name;
    char* line;
    int eof;
};

File* a_file_open(const char* path, const char* modes)
{
    FILE* const handle = fopen(path, modes);

    if(!handle) {
        return NULL;
    }

    File* const f = malloc(sizeof(File));

    f->handle = handle;
    f->modes = a_str_dup(modes);
    f->path = a_str_getPrefixLastFind(path, '/');
    f->name = a_str_getSuffixLastFind(path, '/');
    f->line = NULL;
    f->eof = 0;

    return f;
}

void a_file_close(File* f)
{
    free(f->line);

    if(f->handle) {
        fclose(f->handle);
    }

    free(f);
}

bool a_file_checkPrefix(File* f, const char* prefix)
{
    const int len = strlen(prefix) + 1;
    char buffer[len];

    fseek(f->handle, 0, SEEK_SET);
    fread(buffer, len, 1, f->handle);

    return a_str_equal(buffer, prefix);
}

void a_file_writePrefix(File* f, const char* prefix)
{
    fwrite(prefix, strlen(prefix) + 1, 1, f->handle);
}

void a_file_read(File* f, void* buffer, size_t size)
{
    fread(buffer, size, 1, f->handle);
}

void a_file_write(File* f, void* buffer, size_t size)
{
    fwrite(buffer, size, 1, f->handle);
}

void a_file_writef(File* f, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(f->handle, fmt, args);

    va_end(args);
}

bool a_file_readLine(File* f)
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

        char* const str = malloc(offset * sizeof(char));

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

char* a_file_getLine(const File* f)
{
    return f->line;
}

const char* a_file_path(const File* f)
{
    return f->path;
}

const char* a_file_name(const File* f)
{
    return f->name;
}

FILE* a_file_handle(const File* f)
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

int a_file_size(const char* f)
{
    struct stat info;
    stat(f, &info);

    return info.st_size;
}

uint8_t* a_file_toBuffer(const char* path)
{
    FILE* const f = fopen(path, "r");

    if(!f) {
        return NULL;
    }

    const int len = a_file_size(path);
    uint8_t* const buffer = malloc(len);

    fread(buffer, len, 1, f);
    fclose(f);

    return buffer;
}
