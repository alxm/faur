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

#include "a2x_pack_file.p.h"
#include "a2x_pack_file.v.h"

struct FilePath {
    char* name;
    char* full;
};

struct FileReader {
    File* file;
    char* line;
    int eof;
};

static int defaultSelector(const struct dirent* f);

int a_file_exists(const char* const path)
{
    File* const f = a_file_openRead(path);

    if(f) {
        a_file_close(f);
        return 1;
    }

    return 0;
}

int a_file_dirExists(const char* const path)
{
    Dir* const d = a_file_openDir(path);

    if(d) {
        a_file_closeDir(d);
        return 1;
    }

    return 0;
}

int a_file_hasKey(const char* const path, const char* const key)
{
    File* const f = a_file_openRead(path);

    if(f) {
        char* const buffer = a_str_malloc(key);
        a_file_rp(f, buffer, a_str_size(key));

        int verdict = a_str_same(buffer, key);

        free(buffer);
        a_file_close(f);

        return verdict;
    }

    return 0;
}

int a_file_isDir(const char* const f)
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

List* a_file_list(const char* const path, int (*selector)(const struct dirent* f))
{
    List* const list = a_list_set();

    if(!a_file_dirExists(path)) {
        return list;
    }

    if(!selector) {
        selector = &defaultSelector;
    }

    struct dirent** dlist;
    const int numFiles = scandir(path, &dlist, selector, alphasort);

    for(int i = numFiles; i--; ) {
        FilePath* const f = malloc(sizeof(FilePath));

        f->name = a_str_dup(dlist[i]->d_name);
        f->full = a_str_merge(3, path, "/", f->name);

        a_list_addFirst(list, f);

        free(dlist[i]);
    }

    free(dlist);

    return list;
}

void a_file_freeFilePath(FilePath* const f)
{
    free(f->name);
    free(f->full);

    free(f);
}

char* a_file_pathName(const FilePath* const f)
{
    return f->name;
}

char* a_file_pathFull(const FilePath* const f)
{
    return f->full;
}

FileReader* a_file_makeReader(const char* const file)
{
    FileReader* const f = malloc(sizeof(FileReader));

    f->file = a_file_openReadText(file);
    f->line = NULL;
    f->eof = f->file == NULL;

    return f;
}

void a_file_freeReader(FileReader* const f)
{
    if(f->file) {
        a_file_close(f->file);
    }

    free(f->line);
    free(f);
}

int a_file_readLine(FileReader* const fr)
{
    if(fr->eof) {
        return 0;
    }

    int offset = 1;
    File* const f = fr->file;

    while(offset == 1) {
        int c;

        for(c = fgetc(f); !iscntrl(c) && c != EOF; c = fgetc(f)) {
            offset++;
        }

        if(c == EOF) {
            fr->eof = 1;
            break;
        }
    }

    if(offset > 1) {
        if(fr->eof) {
            a_file_rewind(f);
            a_file_jumpEnd(f, -(offset - 1));
        } else {
            a_file_jump(f, -offset);
        }

        char* const str = malloc(offset * sizeof(char));

        for(int i = 0; i < offset - 1; i++) {
            str[i] = fgetc(f);
        }

        str[offset - 1] = '\0';

        a_file_jump(f, 1);

        free(fr->line);
        fr->line = str;

        return 1;
    }

    return 0;
}

char* a_file_getLine(const FileReader* const fr)
{
    return fr->line;
}

static int defaultSelector(const struct dirent* f)
{
    return a_file_validName(f->d_name);
}
