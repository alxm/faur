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

#if !F_CONFIG_SYSTEM_GAMEBUINO
#include "media/console.png.h"
#include "media/fontgrid.png.h"
#include "media/fontgrid_keyed.png.h"

static AStrHash* g_dirs; // table of AEmbeddedDir
static AStrHash* g_files; // table of AEmbeddedFile

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

    addFile("/faur/consoleTitles", &f__bin__media_console_png);
    addFile("/faur/font", &f__bin__media_fontgrid_png);
    addFile("/faur/fontKeyed", &f__bin__media_fontgrid_keyed_png);

    f_embed__populate();
}

static void f_embed__uninit(void)
{
    f_strhash_free(g_dirs);
    f_strhash_free(g_files);
}

const APack f_pack__embed = {
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
    addDir(((const AEmbeddedDir*)Data)->path, Data);
}

const AEmbeddedDir* f_embed__dirGet(const char* Path)
{
    return f_strhash_get(g_dirs, Path);
}

void f_embed__fileAdd(const void* Data)
{
    addFile(((const AEmbeddedFile*)Data)->path, Data);
}

const AEmbeddedFile* f_embed__fileGet(const char* Path)
{
    return f_strhash_get(g_files, Path);
}
#else // F_CONFIG_SYSTEM_GAMEBUINO
const APack f_pack__embed;
#endif // F_CONFIG_SYSTEM_GAMEBUINO
