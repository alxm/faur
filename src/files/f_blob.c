/*
    Copyright 2020 Alex Margarit <alex@alxm.org>
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

#include "f_blob.v.h"
#include <faur.v.h>

struct FBlob {
    FList* dirs; // FList<FEmbeddedDir*>
    FList* files; // FList<FEmbeddedFile*>
    void* buffer; // NULL if blob itself is an embedded file
};

typedef struct {
    const char* path; // blob file path
    const uint8_t *buffer, *bufferEnd; // blob file buffer
    unsigned alignMask; // used to calculate padding before file buffer start
} FBlobReader;

static uint8_t read_uint8(FBlobReader* Reader)
{
    if(Reader->buffer >= Reader->bufferEnd) {
        F__FATAL("f_blob_new(%s): Not enough data", Reader->path);
    }

    return *Reader->buffer++;
}

static uint32_t read_uint32(FBlobReader* Reader)
{
    return (uint32_t)((read_uint8(Reader) << 24)
                    | (read_uint8(Reader) << 16)
                    | (read_uint8(Reader) <<  8)
                    | (read_uint8(Reader) <<  0));
}

static const char* read_stringz(FBlobReader* Reader)
{
    const char* start = (const char*)Reader->buffer;

    while(read_uint8(Reader) != 0) {
        continue;
    }

    return start;
}

static const uint8_t* read_bytes(FBlobReader* Reader, unsigned Size)
{
    const uint8_t* start = Reader->buffer;

    Reader->buffer += Size;

    if(Reader->buffer > Reader->bufferEnd) {
        F__FATAL("f_blob_new(%s): Not enough data", Reader->path);
    }

    return start;
}

static void read_skip(FBlobReader* Reader, unsigned Size)
{
    Reader->buffer += Size;

    if(Reader->buffer > Reader->bufferEnd) {
        F__FATAL("f_blob_new(%s): Not enough data", Reader->path);
    }
}

static void read_padding(FBlobReader* Reader)
{
    uintptr_t p = (uintptr_t)Reader->buffer;
    uintptr_t mask = Reader->alignMask;

    Reader->buffer = (const uint8_t*)((p + mask) & ~mask);
}

FBlob* f_blob_new(const char* Path)
{
    FFile* f = f_file_new(Path, F_FILE_READ | F_FILE_BINARY);

    if(f == NULL) {
        F__FATAL("f_blob_new(%s): Cannot open file", Path);
    }

    const FPath* path = f_file_pathGet(f);

    void* blobBuffer;
    bool blobBufferEmbedded;
    size_t blobBufferSize = f_path__sizeGet(path);

    if(f_path_test(path, F_PATH_REAL)) {
        blobBuffer = f_mem_malloca(blobBufferSize, F_BLOB__BUFFER_ALIGN_MAX);
        blobBufferEmbedded = false;

        if(!f_file_read(f, blobBuffer, blobBufferSize)) {
            F__FATAL("f_blob_new(%s): Cannot read file", Path);
        }
    } else {
        blobBuffer = (void*)f_embed__fileGet(Path)->buffer;
        blobBufferEmbedded = true;
    }

    f_file_free(f);

    FBlob* b = f_mem_malloc(sizeof(FBlob));

    b->dirs = f_list_new();
    b->files = f_list_new();
    b->buffer = blobBufferEmbedded ? NULL : blobBuffer;

    FBlobReader reader = {
        .path = Path,
        .buffer = blobBuffer,
        .bufferEnd = (const uint8_t*)blobBuffer + blobBufferSize,
        .alignMask = 0
    };

    const char* prefix = "faurblob";

    for(int i = 0; i < 8; i++) {
        if((char)read_uint8(&reader) != prefix[i]) {
            F__FATAL("f_blob_new(%s): Not a blob file", Path);
        }
    }

    // Skip reserved bytes
    for(int i = 8; i--; ) {
        read_uint8(&reader);
    }

    uint32_t numEntries = read_uint32(&reader);
    uint8_t alignExp = read_uint8(&reader);

    if(alignExp > F_BLOB__BUFFER_ALIGN_MAX) {
        F__FATAL("f_blob_new(%s): Cannot align to %d, max %d",
                 Path,
                 1 << alignExp,
                 1 << F_BLOB__BUFFER_ALIGN_MAX);
    }

    reader.alignMask = (1u << alignExp) - 1;

    f_out_info("f_blob_new(%s): %u entries", Path, (unsigned)numEntries);

    while(numEntries--) {
        uint8_t entryType = read_uint8(&reader);
        const char* entryPath = read_stringz(&reader);
        uint32_t entrySize = read_uint32(&reader);

        if(entryType == 1) {
            read_padding(&reader);

            if(f_embed__fileGet(entryPath)) {
                f_out__error("f_blob_new(%s): Entry '%s' already exists",
                             Path,
                             entryPath);

                read_skip(&reader, entrySize);

                continue;
            }

            FEmbeddedFile* emb = f_embed__fileNew(
                                    entryPath,
                                    entrySize,
                                    read_bytes(&reader, entrySize));

            f_list_addLast(b->files, emb);
        } else if(entryType == 2) {
            if(f_embed__dirGet(entryPath)) {
                f_out__error("f_blob_new(%s): Entry '%s' already exists",
                             Path,
                             entryPath);

                read_skip(&reader, entrySize * (unsigned)sizeof(uint32_t));

                continue;
            }

            FEmbeddedDir* emb = f_embed__dirNew(entryPath, entrySize);

            for(uint32_t e = 0; e < entrySize; e++) {
                uint32_t eOffset = read_uint32(&reader);
                const char* ePath = (const char*)blobBuffer + eOffset + 1;

                emb->entries[e] = ePath + f_str_indexGetLast(ePath, '/') + 1;
            }

            f_list_addLast(b->dirs, emb);
        } else {
            F__FATAL("f_blob_new(%s): '%s' has invalid type %u",
                     Path,
                     entryPath,
                     (unsigned)entryType);
        }
    }

    return b;
}

void f_blob_free(FBlob* Blob)
{
    if(Blob == NULL) {
        return;
    }

    f_list_freeEx(Blob->files, (FFree*)f_embed__fileFree);
    f_list_freeEx(Blob->dirs, (FFree*)f_embed__dirFree);

    f_mem_freea(Blob->buffer);
    f_mem_free(Blob);
}
