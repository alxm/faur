/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_FILES_FILE_EMBEDDED_V_H
#define A_INC_FILES_FILE_EMBEDDED_V_H

#include "files/a_file_embedded.p.h"

#include "files/a_file.v.h"

extern AFile* a_file_embedded__new(APath* Path);

extern uint8_t* a_file_embedded__toBuffer(const char* Path);

#endif // A_INC_FILES_FILE_EMBEDDED_V_H
