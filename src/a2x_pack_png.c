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

#include "a2x_pack_png.v.h"

typedef struct ByteStream {
    const uint8_t* data;
    int offset;
} ByteStream;

static void readFunction(png_structp png, png_bytep data, png_size_t length);
static void pngToPixels(png_structp png, png_infop info, Pixel** const pixels, int* const width, int* const height);

void a_png_readFile(const char* const path, Pixel** const pixels, int* const width, int* const height)
{
    File* const f = a_file_open(path, "r");

    png_structp png = NULL;
    png_infop info = NULL;

    if(!f) {
        goto cleanUp;
    }

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    a_file_read(f, sig, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        a_error("%s is not a PNG", path);
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

    png_init_io(png, a_file_file(f));
    png_set_sig_bytes(png, PNG_SIG);
    png_read_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    const int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        a_error("%s not 8-bit RGBA PNG", path);
        goto cleanUp;
    }

    pngToPixels(png, info, pixels, width, height);

    cleanUp:

    if(png) {
        png_destroy_read_struct(&png, info ? &info : NULL, NULL);
    }

    if(f) {
        a_file_close(f);
    }
}

void a_png_readMemory(const uint8_t* const data, Pixel** const pixels, int* const width, int* const height)
{
    ByteStream* const stream = malloc(sizeof(ByteStream));

    stream->data = data;
    stream->offset = 0;

    png_structp png = NULL;
    png_infop info = NULL;

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    memcpy(sig, data, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        a_error("Data not a PNG");
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
    png_read_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    const int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        a_error("Data not 8-bit RGBA PNG");
        goto cleanUp;
    }

    pngToPixels(png, info, pixels, width, height);

    cleanUp:

    if(png) {
        png_destroy_read_struct(&png, info ? &info : NULL, NULL);
    }

    free(stream);
}

void a_png_write(const char* const path, const Pixel* const data, const int width, const int height)
{
    File* f = a_file_open(path, "w");

    png_structp png = NULL;
    png_infop info = NULL;

    png_bytepp rows = malloc(height * sizeof(png_bytep));

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

    png_set_IHDR(
        png, info,
        width, height,
        8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    for(int i = 0; i < height; i++) {
        rows[i] = malloc(width * 3 * sizeof(png_byte));

        for(int j = 0; j < width; j++) {
            const Pixel p = *(data + i * width + j);

            rows[i][j * 3 + 0] = a_pixel_red(p);
            rows[i][j * 3 + 1] = a_pixel_green(p);
            rows[i][j * 3 + 2] = a_pixel_blue(p);
        }
    }

    png_init_io(png, a_file_file(f));
    png_set_rows(png, info, rows);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png, NULL);

    cleanUp:

    if(png) {
        png_destroy_write_struct(&png, info ? &info : NULL);
    }

    for(int i = 0; i < height; i++) {
        free(rows[i]);
    }

    free(rows);
}

static void readFunction(png_structp png, png_bytep data, png_size_t length)
{
    ByteStream* const stream = png_get_io_ptr(png);

    memcpy(data, stream->data + stream->offset, length);
    stream->offset += length;
}

static void pngToPixels(png_structp png, png_infop info, Pixel** const pixels, int* const width, int* const height)
{
    const int w = png_get_image_width(png, info);
    const int h = png_get_image_height(png, info);
    const int channels = png_get_channels(png, info);

    Pixel* const px = malloc(w * h * sizeof(Pixel));
    png_bytepp rows = png_get_rows(png, info);

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            *(px + i * w + j) = a_pixel_make(
                rows[i][j * channels + 0],
                rows[i][j * channels + 1],
                rows[i][j * channels + 2]
            );
        }
    }

    *width = w;
    *height = h;
    *pixels = px;
}
