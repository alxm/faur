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

#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

static bool fileSeek(AFile* File, int Offset, AFileOffset Origin)
{
    bool ret = false;
    size_t off = (size_t)Offset;

    switch(Origin) {
        case A_FILE__OFFSET_START: {
            if(Offset >= 0 && off <= File->u.e.data->size) {
                File->u.e.index = off;
                ret = true;
            }
        } break;

        case A_FILE__OFFSET_CURRENT: {
            if((Offset >= 0 && File->u.e.index + off <= File->u.e.data->size)
                || (Offset < 0 && (size_t)-Offset <= File->u.e.index)) {

                File->u.e.index += off;
                ret = true;
            }
        } break;

        case A_FILE__OFFSET_END: {
            if(Offset <= 0 && (size_t)-Offset <= File->u.e.data->size) {
                File->u.e.index = File->u.e.data->size + off;
                ret = true;
            }
        } break;

        default: break;
    }

    return ret;
}

static bool fileRead(AFile* File, void* Buffer, size_t Size)
{
    size_t len = a_math_minz(Size, File->u.e.data->size - File->u.e.index);

    memcpy(Buffer, File->u.e.data->buffer + File->u.e.index, len);
    File->u.e.index += len;

    return len == Size;
}

static bool fileWrite(AFile* File, const void* Buffer, size_t Size)
{
    A_UNUSED(File);
    A_UNUSED(Buffer);
    A_UNUSED(Size);

    return false;
}

static bool fileWritef(AFile* File, const char* Format, va_list Args)
{
    A_UNUSED(File);
    A_UNUSED(Format);
    A_UNUSED(Args);

    return false;
}

static int fileGetChar(AFile* File)
{
    int c = EOF;

    if(File->u.e.index < File->u.e.data->size) {
        c = File->u.e.data->buffer[File->u.e.index++];
    }

    return c;
}

static int fileUnGetChar(AFile* File, int Char)
{
    A_UNUSED(Char);

    int c = EOF;

    if(File->u.e.index > 0) {
        c = File->u.e.data->buffer[--File->u.e.index];
    }

    return c;
}

static const AFileInterface g_interface = {
    .seek = fileSeek,
    .read = fileRead,
    .write = fileWrite,
    .writef = fileWritef,
    .getchar = fileGetChar,
    .ungetchar = fileUnGetChar,
};

AFile* a_file_embedded__new(APath* Path, AFileMode Mode)
{
    if(Mode & A_FILE_WRITE) {
        a_out__error("a_file_new: Cannot write to embedded file %s",
                     a_path_getFull(Path));
        return NULL;
    }

    AFile* f = a_mem_zalloc(sizeof(AFile));

    f->path = Path;
    f->interface = &g_interface;
    f->u.e.data = a_embed__getFileData(a_path_getFull(Path));
    f->u.e.index = 0;

    return f;
}

uint8_t* a_file_embedded__toBuffer(const char* Path)
{
    const AEmbeddedFile* data = a_embed__getFileData(Path);

    return a_mem_dup(data->buffer, data->size);
}
