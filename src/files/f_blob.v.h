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

#ifndef F_INC_FILES_BLOB_V_H
#define F_INC_FILES_BLOB_V_H

#include "f_blob.p.h"

#include "../data/f_list.v.h"

#define F_BLOB__BUFFER_ALIGN_MAX 4

struct FBlob {
    FList* dirs; // FList<FEmbeddedDir*>
    FList* files; // FList<FEmbeddedFile*>
    void* buffer; // NULL if blob itself is an embedded file
};

#endif // F_INC_FILES_BLOB_V_H
