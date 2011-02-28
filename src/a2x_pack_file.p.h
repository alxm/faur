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

#ifndef A2X_PACK_FILE_PH
#define A2X_PACK_FILE_PH

#include "a2x_app_includes.h"

typedef struct File File;

extern File* a_file_open(const char* const path, const char* const modes);
extern void a_file_close(File* const f);

extern void a_file_read(File* const f, void* const buffer, const size_t size);
extern void a_file_write(File* const f, void* const buffer, const size_t size);

extern int a_file_readLine(File* const f);
extern char* a_file_getLine(const File* const f);

extern const char* a_file_path(const File* const f);
extern const char* a_file_name(const File* const f);
extern FILE* a_file_file(const File* const f);

extern int a_file_exists(const char* const path);
extern int a_file_isDir(const char* const f);
extern int a_file_size(const char* const f);

extern uint8_t* a_file_toBuffer(const char* const path);

#endif // A2X_PACK_FILE_PH
