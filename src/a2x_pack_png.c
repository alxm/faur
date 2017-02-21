/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_pack_png.v.h"

#include <zlib.h>
#include <png.h>

typedef struct AByteStream {
    const uint8_t* data;
    size_t offset;
} AByteStream;

static void readFunction(png_structp Png, png_bytep Data, png_size_t Length)
{
    AByteStream* stream = png_get_io_ptr(Png);

    memcpy(Data, stream->data + stream->offset, Length);
    stream->offset += Length;
}

static void pngToPixels(png_structp Png, png_infop Info, APixel** Pixels, int* Width, int* Height)
{
    png_uint_32 w = png_get_image_width(Png, Info);
    png_uint_32 h = png_get_image_height(Png, Info);
    unsigned numChannels = png_get_channels(Png, Info);

    APixel* pixels = a_mem_malloc(w * h * sizeof(APixel));
    png_bytepp rows = png_get_rows(Png, Info);

    *Width = (int)w;
    *Height = (int)h;
    *Pixels = pixels;

    for(png_uint_32 y = h; y--; rows++) {
        for(png_uint_32 x = w, chOffset = 0; x--; chOffset += numChannels) {
            *pixels++ = a_pixel_make(rows[0][chOffset + 0],
                                     rows[0][chOffset + 1],
                                     rows[0][chOffset + 2]);
        }
    }
}

void a_png_readFile(const char* Path, APixel** Pixels, int* Width, int* Height)
{
    AFile* f = a_file_open(Path, "rb");

    png_structp png = NULL;
    png_infop info = NULL;

    if(!f) {
        goto cleanUp;
    }

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    a_file_read(f, sig, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        a_out__error("File '%s' not a PNG", Path);
        goto cleanUp;
    }

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png) {
        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(!info) {
        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        goto cleanUp;
    }

    png_init_io(png, a_file_handle(f));
    png_set_sig_bytes(png, PNG_SIG);
    png_read_png(png, info, PNG_TRANSFORM_EXPAND, NULL);

    int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        a_out__error("File '%s' not an RGB or RGBA PNG", Path);
        goto cleanUp;
    }

    pngToPixels(png, info, Pixels, Width, Height);

cleanUp:
    if(png) {
        png_destroy_read_struct(&png, info ? &info : NULL, NULL);
    }

    if(f) {
        a_file_close(f);
    }
}

void a_png_readMemory(const uint8_t* Data, APixel** Pixels, int* Width, int* Height)
{
    AByteStream* stream = a_mem_malloc(sizeof(AByteStream));

    stream->data = Data;
    stream->offset = 0;

    png_structp png = NULL;
    png_infop info = NULL;

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    memcpy(sig, Data, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        a_out__error("Data not a PNG");
        goto cleanUp;
    }

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png) {
        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(!info) {
        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        goto cleanUp;
    }

    png_set_read_fn(png, stream, readFunction);
    png_read_png(png, info, PNG_TRANSFORM_EXPAND, NULL);

    const int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        a_out__error("Data not an RGB or RGBA PNG");
        goto cleanUp;
    }

    pngToPixels(png, info, Pixels, Width, Height);

cleanUp:
    if(png) {
        png_destroy_read_struct(&png, info ? &info : NULL, NULL);
    }

    free(stream);
}

void a_png_write(const char* Path, const APixel* Data, int Width, int Height, char* Title, char* Description)
{
    AFile* f = a_file_open(Path, "wb");

    png_structp png = NULL;
    png_infop info = NULL;
    volatile png_bytepp rows = NULL;
    png_text text[2];
    volatile int numText = 0;

    if(!f) {
        goto cleanUp;
    }

    rows = a_mem_malloc((unsigned)Height * sizeof(png_bytep));
    memset(rows, 0, (unsigned)Height * sizeof(png_bytep));

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png) {
        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(!info) {
        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        goto cleanUp;
    }

    png_set_compression_level(png, Z_BEST_COMPRESSION);

    png_set_IHDR(png,
                 info,
                 (unsigned)Width,
                 (unsigned)Height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    for(int i = 0; i < Height; i++) {
        rows[i] = a_mem_malloc((unsigned)Width * 3 * sizeof(png_byte));

        for(int j = 0; j < Width; j++) {
            const APixel p = *(Data + i * Width + j);

            rows[i][j * 3 + 0] = (png_byte)a_pixel_red(p);
            rows[i][j * 3 + 1] = (png_byte)a_pixel_green(p);
            rows[i][j * 3 + 2] = (png_byte)a_pixel_blue(p);
        }
    }

    if(Title != NULL) {
        text[numText].compression = PNG_TEXT_COMPRESSION_NONE;
        text[numText].key = "Title";
        text[numText].text = Title;
        numText++;
    }

    if(Description != NULL) {
        text[numText].compression = PNG_TEXT_COMPRESSION_NONE;
        text[numText].key = "Description";
        text[numText].text = Description;
        numText++;
    }

    png_init_io(png, a_file_handle(f));
    png_set_rows(png, info, rows);
    png_set_text(png, info, text, numText);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png, NULL);

cleanUp:
    if(png) {
        png_destroy_write_struct(&png, info ? &info : NULL);
    }

    if(rows) {
        for(int i = 0; i < Height; i++) {
            free(rows[i]);
        }

        free(rows);
    }

    if(f) {
        a_file_close(f);
    }
}
