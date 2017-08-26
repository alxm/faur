/*
    Copyright 2017 Alex Margarit

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

#include "a2x_pack_embed.v.h"

#include "media/console.h"
#include "media/font.h"

typedef struct AEmbeddedData {
    const uint8_t* buffer;
    size_t size;
} AEmbeddedData;

AStrHash* g_data; // table of AEmbeddedData

void a_embed__init(void)
{
    g_data = a_strhash_new();

    a_embed__add("/a2x/consoleTitles", g_media_console, g_media_console_size);
    a_embed__add("/a2x/defaultFont", g_media_font, g_media_font_size);

    a_embed__application();
}

void a_embed__uninit(void)
{
    a_strhash_freeEx(g_data, free);
}

void a_embed__add(const char* Key, const uint8_t* Buffer, size_t Size)
{
    AEmbeddedData* data = a_mem_malloc(sizeof(AEmbeddedData));

    data->buffer = Buffer;
    data->size = Size;

    a_strhash_add(g_data, Key, data);
}

bool a_embed__get(const char* Key, const uint8_t** Buffer, size_t* Size)
{
    AEmbeddedData* data = a_strhash_get(g_data, Key);

    if(data) {
        if(Buffer) {
            *Buffer = data->buffer;
        }

        if(Size) {
            *Size = data->size;
        }
    }

    return data != NULL;
}
