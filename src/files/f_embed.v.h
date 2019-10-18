/*
    Copyright 2017, 2019 Alex Margarit <alex@alxm.org>
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

#include "files/f_embed.p.h"

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

#include "general/f_main.v.h"

extern const APack f_pack__embed;

extern void f_embed__populate(void);

extern void f_embed__dirAdd(const void* Data);
extern const AEmbeddedDir* f_embed__dirGet(const char* Path);

extern void f_embed__fileAdd(const void* Data);
extern const AEmbeddedFile* f_embed__fileGet(const char* Path);

#endif // F_INC_FILES_EMBED_V_H
