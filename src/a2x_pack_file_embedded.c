/*
    Copyright 2018 Alex Margarit

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

#include "a2x_pack_file_embedded.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

AFile* a_file_embedded__new(APath* Path, AFileMode Mode)
{
    if(Mode & A_FILE_WRITE) {
        a_out__error("a_file_new: Can't write to embedded file %s",
                     a_path_getFull(Path));
        return NULL;
    }

    AFile* f = a_mem_zalloc(sizeof(AFile));

    f->path = Path;
    f->interface = NULL;
    f->u.data = a_embed__getFileData(a_path_getFull(Path));

    return f;
}

uint8_t* a_file_embedded__toBuffer(const char* Path)
{
    const AEmbeddedFile* data = a_embed__getFileData(Path);

    return a_mem_dup(data->buffer, data->size);
}
