/*
    Copyright 2017-2021 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_FILES_EMBED_PATHS_BLOB || F_CONFIG_FILES_EMBED_PATHS_C
static FHash* g_dirs; // FHash<const char*, FEmbeddedDir>
static FHash* g_files; // FHash<const char*, FEmbeddedFile*>

#if F_CONFIG_FILES_EMBED_PATHS_BLOB
    static FBlob* g_blob;
#endif

#if F_CONFIG_TRAIT_LOW_MEM
    #define F__EMBED_HASH_SLOTS 8
#else
    #define F__EMBED_HASH_SLOTS 256
#endif

static void f_embed__init(void)
{
    g_dirs = f_hash_newStr(F__EMBED_HASH_SLOTS, false);
    g_files = f_hash_newStr(F__EMBED_HASH_SLOTS, false);

    #if F_CONFIG_FILES_EMBED_PATHS_C
        f_embed__populate();
    #endif

    #if F_CONFIG_FILES_EMBED_PATHS_BLOB
        if(f_path_exists(F_CONFIG_FILES_EMBED_BLOB, F_PATH_FILE)) {
            g_blob = f_blob_new(F_CONFIG_FILES_EMBED_BLOB);
        }
    #endif
}

static void f_embed__uninit(void)
{
    #if F_CONFIG_FILES_EMBED_PATHS_BLOB
        f_blob_free(g_blob);
    #endif

    f_hash_free(g_dirs);
    f_hash_free(g_files);
}

const FPack f_pack__embed = {
    "Embed",
    f_embed__init,
    f_embed__uninit,
};

void f_embed__set(const FEmbeddedDir* const* Dirs, size_t DirsNum, const FEmbeddedFile* const* Files, size_t FilesNum)
{
    for(size_t d = DirsNum; d--; ) {
        f_hash_add(g_dirs, Dirs[d]->path, (void*)Dirs[d]);
    }

    for(size_t f = FilesNum; f--; ) {
        f_hash_add(g_files, Files[f]->path, (void*)Files[f]);
    }
}

FEmbeddedDir* f_embed__dirNew(const char* Path, size_t Size)
{
    FEmbeddedDir* d = f_mem_malloc(sizeof(FEmbeddedDir));

    d->path = Path;
    d->size = Size;
    d->entries = f_mem_malloc(Size * sizeof(const char*));

    f_hash_add(g_dirs, Path, d);

    return d;
}

void f_embed__dirFree(FEmbeddedDir* Dir)
{
    f_hash_removeKey(g_dirs, Dir->path);

    f_mem_free(Dir->entries);
    f_mem_free(Dir);
}

const FEmbeddedDir* f_embed__dirGet(const char* Path)
{
    return f_hash_get(g_dirs, Path);
}

FEmbeddedFile* f_embed__fileNew(const char* Path, size_t Size, const uint8_t* Buffer)
{
    FEmbeddedFile* f = f_mem_malloc(sizeof(FEmbeddedFile));

    f->path = Path;
    f->size = Size;
    f->buffer = Buffer;

    f_hash_add(g_files, Path, f);

    return f;
}

void f_embed__fileFree(FEmbeddedFile* File)
{
    f_hash_removeKey(g_files, File->path);

    f_mem_free(File);
}

const FEmbeddedFile* f_embed__fileGet(const char* Path)
{
    return f_hash_get(g_files, Path);
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
#else // !(F_CONFIG_FILES_EMBED_PATHS_BLOB || F_CONFIG_FILES_EMBED_PATHS_C)
bool f_embed__stat(const char* Path, FPathInfo* Info)
{
    F_UNUSED(Path);
    F_UNUSED(Info);

    return false;
}

const FEmbeddedDir* f_embed__dirGet(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

const FEmbeddedFile* f_embed__fileGet(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}
#endif // !(F_CONFIG_FILES_EMBED_PATHS_BLOB || F_CONFIG_FILES_EMBED_PATHS_C)
