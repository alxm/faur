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
    FILE* file;
    char* modes;
    char* path;
    char* name;
    char* line;
    int eof;
};

File* a_file_open(const char* const path, const char* const modes)
{
    FILE* const file = fopen(path, modes);

    if(!file) {
        return NULL;
    }

    File* const f = malloc(sizeof(File));

    f->file = file;
    f->modes = a_str_dup(modes);
    f->path = a_str_getPrefixLastFind(path, '/');
    f->name = a_str_getSuffixLastFind(path, '/');
    f->line = NULL;
    f->eof = 0;

    return f;
}

void a_file_close(File* const f)
{
    if(f->file) {
        fclose(f->file);
    }

    free(f->modes);
    free(f->path);
    free(f->name);
    free(f->line);

    free(f);
}

bool a_file_checkPrefix(File* const f, const char* const prefix)
{
    const int len = strlen(prefix);

    char buffer[len + 1];
    buffer[len] = '\0';

    fseek(f->file, 0, SEEK_SET);
    fread(buffer, len, 1, f->file);

    return a_str_equal(buffer, prefix);
}

void a_file_read(File* const f, void* const buffer, const size_t size)
{
    fread(buffer, size, 1, f->file);
}

void a_file_write(File* const f, void* const buffer, const size_t size)
{
    fwrite(buffer, size, 1, f->file);
}

bool a_file_readLine(File* const f)
{
    if(f->eof) {
        return false;
    }

    int offset = 1;
    FILE* const file = f->file;

    while(offset == 1 && !f->eof) {
        int c;

        for(c = fgetc(file); !iscntrl(c) && c != EOF; c = fgetc(file)) {
            offset++;
        }

        f->eof = c == EOF;
    }

    if(offset > 1) {
        if(f->eof) {
            rewind(file);
            fseek(file, -(offset - 1), SEEK_END);
        } else {
            fseek(file, -offset, SEEK_CUR);
        }

        char* const str = malloc(offset * sizeof(char));

        for(int i = 0; i < offset - 1; i++) {
            str[i] = fgetc(file);
        }

        str[offset - 1] = '\0';

        fseek(file, 1, SEEK_CUR);

        free(f->line);
        f->line = str;

        return true;
    }

    return false;
}

char* a_file_getLine(const File* const f)
{
    return f->line;
}

const char* a_file_path(const File* const f)
{
    return f->path;
}

const char* a_file_name(const File* const f)
{
    return f->name;
}

FILE* a_file_file(const File* const f)
{
    return f->file;
}

bool a_file_exists(const char* const path)
{
    FILE* const f = fopen(path, "r");

    if(f) {
        fclose(f);
        return true;
    }

    return false;
}

bool a_file_isDir(const char* const f)
{
    struct stat info;
    stat(f, &info);

    return S_ISDIR(info.st_mode);
}

int a_file_size(const char* const f)
{
    struct stat info;
    stat(f, &info);

    return info.st_size;
}

uint8_t* a_file_toBuffer(const char* const path)
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
