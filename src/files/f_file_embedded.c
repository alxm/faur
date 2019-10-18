/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "f_file_embedded.v.h"
#include <faur.v.h>

static bool fileSeek(FFile* File, int Offset, FFileOffset Origin)
{
    bool ret = false;
    size_t off = (size_t)Offset;

    switch(Origin) {
        case F_FILE__OFFSET_START: {
            if(Offset >= 0 && off <= File->u.e.data->size) {
                File->u.e.index = off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_CURRENT: {
            if((Offset >= 0 && File->u.e.index + off <= File->u.e.data->size)
                || (Offset < 0 && (size_t)-Offset <= File->u.e.index)) {

                File->u.e.index += off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_END: {
            if(Offset <= 0 && (size_t)-Offset <= File->u.e.data->size) {
                File->u.e.index = File->u.e.data->size + off;
                ret = true;
            }
        } break;

        default: break;
    }

    return ret;
}

static bool fileRead(FFile* File, void* Buffer, size_t Size)
{
    size_t len = f_math_minz(Size, File->u.e.data->size - File->u.e.index);

    memcpy(Buffer, File->u.e.data->buffer + File->u.e.index, len);
    File->u.e.index += len;

    return len == Size;
}

static bool fileWrite(FFile* File, const void* Buffer, size_t Size)
{
    F_UNUSED(File);
    F_UNUSED(Buffer);
    F_UNUSED(Size);

    return false;
}

static bool fileWritef(FFile* File, const char* Format, va_list Args)
{
    F_UNUSED(File);
    F_UNUSED(Format);
    F_UNUSED(Args);

    return false;
}

static bool fileFlush(FFile* File)
{
    F_UNUSED(File);

    return false;
}

static int fileGetChar(FFile* File)
{
    int c = EOF;

    if(File->u.e.index < File->u.e.data->size) {
        c = File->u.e.data->buffer[File->u.e.index++];
    }

    return c;
}

static int fileUnGetChar(FFile* File, int Char)
{
    F_UNUSED(Char);

    int c = EOF;

    if(File->u.e.index > 0) {
        c = File->u.e.data->buffer[--File->u.e.index];
    }

    return c;
}

static const FFileInterface g_interface = {
    .seek = fileSeek,
    .read = fileRead,
    .write = fileWrite,
    .writef = fileWritef,
    .flush = fileFlush,
    .getchar = fileGetChar,
    .ungetchar = fileUnGetChar,
};

FFile* f_file_embedded__new(FPath* Path)
{
    FFile* f = f_mem_zalloc(sizeof(FFile));

    f->path = Path;
    f->interface = &g_interface;
    f->u.e.data = f_embed__fileGet(f_path_getFull(Path));
    f->u.e.index = 0;

    return f;
}

uint8_t* f_file_embedded__toBuffer(const char* Path)
{
    const FEmbeddedFile* data = f_embed__fileGet(Path);

    return f_mem_dup(data->buffer, data->size);
}
