/*
    Copyright 2017 Alex Margarit <alex@alxm.org>
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

static FHash* g_dirs; // FHash<const char*, FEmbeddedDir>
static FHash* g_files; // FHash<const char*, FEmbeddedFile*>

#if F_CONFIG_FILES_EMBED_BLOB
    static FBlob* g_blob;
#endif

#if F_CONFIG_TRAIT_LOW_MEM
    #define F__EMBED_HASH_SLOTS 8
#else
    #define F__EMBED_HASH_SLOTS 256
#endif

static void f_embed__init(void)
{
    if(f__embed_dirs[0] != NULL) {
        g_dirs = f_hash_newStr(F__EMBED_HASH_SLOTS, false);
    }

    for(const FEmbeddedDir** d = &f__embed_dirs[0]; *d != NULL; d++) {
        f_hash_add(g_dirs, (*d)->path, (void*)(*d));
    }

    if(f__embed_files[0] != NULL) {
        g_files = f_hash_newStr(F__EMBED_HASH_SLOTS, false);
    }

    for(const FEmbeddedFile** f = &f__embed_files[0]; *f != NULL; f++) {
        f_hash_add(g_files, (*f)->path, (void*)(*f));
    }

    #if F_CONFIG_FILES_EMBED_BLOB
        g_blob = f_blob_new(F_CONFIG_FILES_EMBED_BLOB_FILE);
    #endif
}

static void f_embed__uninit(void)
{
    #if F_CONFIG_FILES_EMBED_BLOB
        f_blob_free(g_blob);
    #endif

    f_hash_free(g_dirs);
    f_hash_free(g_files);
}

const FPack f_pack__embed = {
    "Embedded files",
    f_embed__init,
    f_embed__uninit,
};

FEmbeddedDir* f_embed__dirNew(const char* Path, size_t Size)
{
    FEmbeddedDir* d = f_mem_malloc(sizeof(FEmbeddedDir));

    d->path = Path;
    d->size = Size;
    d->entries = f_mem_malloc(Size * sizeof(const char*));

    if(g_dirs == NULL) {
        g_dirs = f_hash_newStr(F__EMBED_HASH_SLOTS, false);
    }

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
    return g_dirs ? f_hash_get(g_dirs, Path) : NULL;
}

FEmbeddedFile* f_embed__fileNew(const char* Path, size_t Size, const uint8_t* Buffer)
{
    FEmbeddedFile* f = f_mem_malloc(sizeof(FEmbeddedFile));

    f->path = Path;
    f->size = Size;
    f->buffer = Buffer;

    if(g_files == NULL) {
        g_files = f_hash_newStr(F__EMBED_HASH_SLOTS, false);
    }

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
    return g_files ? f_hash_get(g_files, Path) : NULL;
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

FFileEmbedded* f_file_embedded__new(const FPath* Path)
{
    FFileEmbedded* f = f_mem_malloc(sizeof(FFileEmbedded));

    f->data = f_embed__fileGet(f_path_getFull(Path));
    f->index = 0;

    return f;
}

void f_file_embedded__free(FFileEmbedded* File)
{
    if(File == NULL) {
        return;
    }

    f_mem_free(File);
}

bool f_file_embedded__seek(FFileEmbedded* File, int Offset, FFileOffset Origin)
{
    bool ret = false;
    size_t off = (size_t)Offset;

    switch(Origin) {
        case F_FILE__OFFSET_START: {
            if(Offset >= 0 && off <= File->data->size) {
                File->index = off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_CURRENT: {
            if((Offset >= 0 && File->index + off <= File->data->size)
                || (Offset < 0 && (size_t)-Offset <= File->index)) {

                File->index += off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_END: {
            if(Offset <= 0 && (size_t)-Offset <= File->data->size) {
                File->index = File->data->size + off;
                ret = true;
            }
        } break;

        default: break;
    }

    return ret;
}

bool f_file_embedded__read(FFileEmbedded* File, void* Buffer, size_t Size)
{
    size_t len = f_math_minz(Size, File->data->size - File->index);

    memcpy(Buffer, File->data->buffer + File->index, len);
    File->index += len;

    return len == Size;
}

int f_file_embedded__readChar(FFileEmbedded* File)
{
    int c = EOF;

    if(File->index < File->data->size) {
        c = File->data->buffer[File->index++];
    }

    return c;
}

int f_file_embedded__readCharUndo(FFileEmbedded* File, int Char)
{
    F_UNUSED(Char);

    int c = EOF;

    if(File->index > 0) {
        c = File->data->buffer[--File->index];
    }

    return c;
}

bool f_file_embedded__bufferRead(const char* Path, void* Buffer, size_t Size)
{
    const FEmbeddedFile* data = f_embed__fileGet(Path);

    if(data && data->size <= Size) {
        memcpy(Buffer, data->buffer, data->size);

        return true;
    }

    return false;
}
