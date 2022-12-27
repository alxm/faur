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

#ifndef F_INC_FILES_EMBED_V_H
#define F_INC_FILES_EMBED_V_H

#include "f_embed.p.h"

typedef struct FEmbeddedDir FEmbeddedDir;
typedef struct FEmbeddedFile FEmbeddedFile;
typedef struct FFileEmbedded FFileEmbedded;

#include "../files/f_file.v.h"
#include "../files/f_path.v.h"
#include "../general/f_init.v.h"

struct FEmbeddedDir {
    const char* path;
    size_t size;
    const char** entries;
};

struct FEmbeddedFile {
    const char* path;
    size_t size;
    const uint8_t* buffer;
};

struct FFileEmbedded {
    const FEmbeddedFile* data;
    size_t index;
};

extern const FPack f_pack__embed;

extern const FEmbeddedDir* f__embed_dirs[];
extern const FEmbeddedFile* f__embed_files[];

extern FEmbeddedDir* f_embed__dirNew(const char* Path, size_t Size);
extern void f_embed__dirFree(FEmbeddedDir* Dir);
extern const FEmbeddedDir* f_embed__dirGet(const char* Path);

extern FEmbeddedFile* f_embed__fileNew(const char* Path, size_t Size, const uint8_t* Buffer);
extern void f_embed__fileFree(FEmbeddedFile* File);
extern const FEmbeddedFile* f_embed__fileGet(const char* Path);

extern bool f_embed__stat(const char* Path, FPathInfo* Info);

extern FFileEmbedded* f_file_embedded__new(const FPath* Path);
extern void f_file_embedded__free(FFileEmbedded* File);

extern bool f_file_embedded__seek(FFileEmbedded* File, int Offset, FFileOffset Origin);
extern bool f_file_embedded__read(FFileEmbedded* File, void* Buffer, size_t Size);
extern int f_file_embedded__readChar(FFileEmbedded* File);
extern int f_file_embedded__readCharUndo(FFileEmbedded* File, int Char);

extern bool f_file_embedded__bufferRead(const char* Path, void* Buffer, size_t Size);

#endif // F_INC_FILES_EMBED_V_H
