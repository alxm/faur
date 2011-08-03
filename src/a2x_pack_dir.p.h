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

#ifndef A2X_PACK_DIR_PH
#define A2X_PACK_DIR_PH

#include "a2x_app_includes.h"

typedef struct Dir Dir;

extern Dir* a_dir_open(const char* const path);
extern Dir* a_dir_openFilter(const char* const path, int (*filter)(const struct dirent* f));
extern void a_dir_close(Dir* const d);

extern void a_dir_reverse(Dir* const d);

extern bool a_dir_iterate(Dir* const d);
extern const char** a_dir_current(const Dir* const d);

#define DirIterate(dir, name, path)                                                     \
    for(const char** a__pair; a_dir_iterate(dir) && (a__pair = a_dir_current(dir)); )   \
        for(const char const *name = a__pair[0], *path = a__pair[1]; name; name = NULL)

extern const char* a_dir_path(const Dir* const d);
extern const char* a_dir_name(const Dir* const d);
extern int a_dir_num(const Dir* const d);

extern bool a_dir_exists(const char* const path);
extern void a_dir_make(const char* const path);

#endif // A2X_PACK_DIR_PH
