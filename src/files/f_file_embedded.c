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

struct FFileEmbedded {
    const FEmbeddedFile* data;
    size_t index;
};

FFileEmbedded* f_file_embedded__new(FPath* Path)
{
    FFileEmbedded* f = f_mem_malloc(sizeof(FFileEmbedded));

    f->data = f_embed__fileGet(f_path_getFull(Path));
    f->index = 0;

    return f;
}

bool f_file_embedded__seek(FFileEmbedded* File, int Offset, FFileOffset Origin)
{
    bool ret = false;
    size_t off = (size_t)Offset;

    switch(Origin) {
        case F_FILE__OFFSET_START: {
            if(Offset >= 0 && off <= File->data->size) {
                File->index = off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_CURRENT: {
            if((Offset >= 0 && File->index + off <= File->data->size)
                || (Offset < 0 && (size_t)-Offset <= File->index)) {

                File->index += off;
                ret = true;
            }
        } break;

        case F_FILE__OFFSET_END: {
            if(Offset <= 0 && (size_t)-Offset <= File->data->size) {
                File->index = File->data->size + off;
                ret = true;
            }
        } break;

        default: break;
    }

    return ret;
}

bool f_file_embedded__read(FFileEmbedded* File, void* Buffer, size_t Size)
{
    size_t len = f_math_minz(Size, File->data->size - File->index);

    memcpy(Buffer, File->data->buffer + File->index, len);
    File->index += len;

    return len == Size;
}

bool f_file_embedded__write(FFileEmbedded* File, const void* Buffer, size_t Size)
{
    F_UNUSED(File);
    F_UNUSED(Buffer);
    F_UNUSED(Size);

    return false;
}

bool f_file_embedded__writef(FFileEmbedded* File, const char* Format, va_list Args)
{
    F_UNUSED(File);
    F_UNUSED(Format);
    F_UNUSED(Args);

    return false;
}

bool f_file_embedded__flush(FFileEmbedded* File)
{
    F_UNUSED(File);

    return false;
}

int f_file_embedded__readChar(FFileEmbedded* File)
{
    int c = EOF;

    if(File->index < File->data->size) {
        c = File->data->buffer[File->index++];
    }

    return c;
}

int f_file_embedded__readCharUndo(FFileEmbedded* File, int Char)
{
    F_UNUSED(Char);

    int c = EOF;

    if(File->index > 0) {
        c = File->data->buffer[--File->index];
    }

    return c;
}

uint8_t* f_file_embedded__toBuffer(const char* Path)
{
    const FEmbeddedFile* data = f_embed__fileGet(Path);

    return f_mem_dup(data->buffer, data->size);
}
