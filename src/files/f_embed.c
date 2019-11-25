/*
    Copyright 2017-2019 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_FILES_EMBED
static FStrHash* g_dirs; // table of FEmbeddedDir
static FStrHash* g_files; // table of FEmbeddedFile

static inline void addDir(const char* Path, const void* Data)
{
    f_strhash_add(g_dirs, Path, (void*)Data);
}

static inline void addFile(const char* Path, const void* Data)
{
    f_strhash_add(g_files, Path, (void*)Data);
}

static void f_embed__init(void)
{
    g_dirs = f_strhash_new();
    g_files = f_strhash_new();

    f_embed__populate();
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

void f_embed__dirAdd(const void* Data)
{
    addDir(((const FEmbeddedDir*)Data)->path, Data);
}

const FEmbeddedDir* f_embed__dirGet(const char* Path)
{
    return f_strhash_get(g_dirs, Path);
}

void f_embed__fileAdd(const void* Data)
{
    addFile(((const FEmbeddedFile*)Data)->path, Data);
}

const FEmbeddedFile* f_embed__fileGet(const char* Path)
{
    return f_strhash_get(g_files, Path);
}
#else // !F_CONFIG_FILES_EMBED
const FPack f_pack__embed;

void f_embed__dirAdd(const void* Data)
{
    F_UNUSED(Data);
}

const FEmbeddedDir* f_embed__dirGet(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

void f_embed__fileAdd(const void* Data)
{
    F_UNUSED(Data);
}

const FEmbeddedFile* f_embed__fileGet(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}
#endif // !F_CONFIG_FILES_EMBED
