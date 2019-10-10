/*
    Copyright 2017, 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_FILES_EMBED_V_H
#define A_INC_FILES_EMBED_V_H

#include "files/a_embed.p.h"

typedef struct {
    const char* path;
    size_t size;
    const char* entries[];
} AEmbeddedDir;

typedef struct {
    const char* path;
    size_t size;
    uint8_t buffer[];
} AEmbeddedFile;

#include "general/a_main.v.h"

extern const APack a_pack__embed;

extern void a_embed__populate(void);

extern void a_embed__dirAdd(const void* Data);
extern const AEmbeddedDir* a_embed__dirGet(const char* Path);

extern void a_embed__fileAdd(const void* Data);
extern const AEmbeddedFile* a_embed__fileGet(const char* Path);

#endif // A_INC_FILES_EMBED_V_H
