/*
    Copyright 2017-2020 Alex Margarit <alex@alxm.org>
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

#include "f_embed.v.h"
#include <faur.v.h>

static FStrHash* g_dirs; // table of FEmbeddedDir
static FStrHash* g_files; // table of FEmbeddedFile

static void f_embed__init(void)
{
    g_dirs = f_strhash_new();
    g_files = f_strhash_new();

    #if F_CONFIG_FILES_EMBED
        f_embed__populate();
    #endif
}

static void f_embed__uninit(void)
{
    f_strhash_free(g_dirs);
    f_strhash_free(g_files);
}

const FPack f_pack__embed = {
    "Embed",
    {
        [0] = f_embed__init,
    },
    {
        [0] = f_embed__uninit,
    },
};

void f_embed__dirAdd(const FEmbeddedDir* Dir)
{
    f_strhash_add(g_dirs, Dir->path, (void*)Dir);
}

const FEmbeddedDir* f_embed__dirGet(const char* Path)
{
    return f_strhash_get(g_dirs, Path);
}

void f_embed__fileAdd(const FEmbeddedFile* File)
{
    f_strhash_add(g_files, File->path, (void*)File);
}

const FEmbeddedFile* f_embed__fileGet(const char* Path)
{
    return f_strhash_get(g_files, Path);
}

bool f_embed__stat(const char* Path, FPathInfo* Info)
{
    const FEmbeddedFile* f = f_embed__fileGet(Path);

    if(f) {
        Info->flags = F_PATH_EMBEDDED | F_PATH_FILE;
        Info->size = f->size;

        return true;
    }

    const FEmbeddedDir* d = f_embed__dirGet(Path);

    if(d) {
        Info->flags = F_PATH_EMBEDDED | F_PATH_DIR;
        Info->size = 0;

        return true;
    }

    return false;
}
