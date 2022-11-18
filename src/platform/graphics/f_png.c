/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_png.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_PNG
#include <zlib.h>
#include <png.h>

typedef struct {
    const uint8_t* data;
    size_t offset;
} FByteStream;

static void readFunction(png_structp Png, png_bytep Data, png_size_t Length)
{
    FByteStream* stream = png_get_io_ptr(Png);

    memcpy(Data, stream->data + stream->offset, Length);
    stream->offset += Length;
}

static FPixels* pngToPixels(png_structp Png, png_infop Info)
{
    png_uint_32 w = png_get_image_width(Png, Info);
    png_uint_32 h = png_get_image_height(Png, Info);
    unsigned numChannels = png_get_channels(Png, Info);
    png_bytepp rows = png_get_rows(Png, Info);

    FPixels* pixels = f_pixels__new((int)w, (int)h, 1, F_PIXELS__ALLOC);
    FColorPixel* buffer = pixels->u.buffer;

    for(png_uint_32 y = h; y--; rows++) {
        for(png_uint_32 x = w, chOffset = 0; x--; chOffset += numChannels) {
            *buffer++ = f_color_pixelFromRgb3(rows[0][chOffset + 0],
                                              rows[0][chOffset + 1],
                                              rows[0][chOffset + 2]);
        }
    }

    return pixels;
}

static FPixels* f_png__readFile(const char* Path)
{
    FPixels* volatile pixels = NULL;
    png_structp png = NULL;
    png_infop info = NULL;
    FFile* f = f_file_new(Path, F_FILE_READ | F_FILE_BINARY);

    if(f == NULL) {
        f_out__error("f_png__readFile(%s): Cannot open file", Path);

        goto cleanUp;
    }

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    f_file_read(f, sig, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        f_out__error("png_sig_cmp(%s) failed", Path);

        goto cleanUp;
    }

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(png == NULL) {
        f_out__error("png_create_read_struct(%s) failed", Path);

        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(info == NULL) {
        f_out__error("png_create_info_struct(%s) failed", Path);

        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        if(pixels != NULL) {
            f_pixels__free(pixels);
            pixels = NULL;
        }

        f_out__error("f_png__readFile(%s) failed", Path);

        goto cleanUp;
    }

    png_init_io(png, f_file_handleGet(f));
    png_set_sig_bytes(png, PNG_SIG);
    png_read_png(png, info, PNG_TRANSFORM_EXPAND, NULL);

    int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        f_out__error("png_get_color_type(%s): Not an RGB or RGBA PNG", Path);

        goto cleanUp;
    }

    pixels = pngToPixels(png, info);

cleanUp:
    if(png != NULL) {
        png_destroy_read_struct(&png, &info, NULL);
    }

    f_file_free(f);

    return pixels;
}

static FPixels* f_png__readMemory(const uint8_t* Data)
{
    FPixels* volatile pixels = NULL;
    png_structp png = NULL;
    png_infop info = NULL;
    FByteStream stream = {Data, 0};

    #define PNG_SIG 8
    png_byte sig[PNG_SIG];

    memcpy(sig, Data, PNG_SIG);

    if(png_sig_cmp(sig, 0, PNG_SIG) != 0) {
        f_out__error("png_sig_cmp failed");

        goto cleanUp;
    }

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(png == NULL) {
        f_out__error("png_create_read_struct failed");

        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(info == NULL) {
        f_out__error("png_create_info_struct failed");

        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        if(pixels != NULL) {
            f_pixels__free(pixels);
            pixels = NULL;
        }

        f_out__error("f_png__readMemory failed");

        goto cleanUp;
    }

    png_set_read_fn(png, &stream, readFunction);
    png_read_png(png, info, PNG_TRANSFORM_EXPAND, NULL);

    const int type = png_get_color_type(png, info);

    if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGBA) {
        f_out__error("png_get_color_type: Not an RGB or RGBA PNG");

        goto cleanUp;
    }

    pixels = pngToPixels(png, info);

cleanUp:
    if (png != NULL) {
        png_destroy_read_struct(&png, &info, NULL);
    }

    return pixels;
}

FPixels* f_png__read(const char* Path)
{
    FPixels* pixels = NULL;

    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        pixels = f_png__readFile(Path);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        pixels = f_png__readMemory(f_embed__fileGet(Path)->buffer);
    }

    return pixels;
}

void f_png__write(const char* Path, const FPixels* Pixels, unsigned Frame, char* Title, char* Description)
{
    png_structp png = NULL;
    png_infop info = NULL;
    volatile png_bytepp rows = NULL;
    volatile png_bytep rowsData = NULL;
    png_text text[2];
    volatile int numText = 0;

    FFile* f = f_file_new(Path, F_FILE_WRITE | F_FILE_BINARY);

    if(f == NULL) {
        goto cleanUp;
    }

    unsigned width = (unsigned)Pixels->size.x;
    unsigned height = (unsigned)Pixels->size.y;

    #define COLOR_CHANNELS 3
    rows = f_mem_malloc(height * sizeof(png_bytep));
    rowsData = f_mem_malloc(width * height * COLOR_CHANNELS * sizeof(png_byte));

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(png == NULL) {
        f_out__error("png_create_write_struct(%s) failed", Path);

        goto cleanUp;
    }

    info = png_create_info_struct(png);

    if(info == NULL) {
        f_out__error("png_create_info_struct(%s) failed", Path);

        goto cleanUp;
    }

    if(setjmp(png_jmpbuf(png))) {
        f_out__error("f_png__write(%s) failed", Path);

        goto cleanUp;
    }

    png_set_compression_level(png, Z_BEST_COMPRESSION);

    png_set_IHDR(png,
                 info,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    const FColorPixel* buffer = f_pixels__bufferGetStartConst(Pixels, Frame);

    for(unsigned y = 0; y < height; y++) {
        rows[y] = rowsData + y * width * COLOR_CHANNELS;

        for(unsigned x = 0; x < width; x++) {
            FColorRgb rgb = f_color_pixelToRgb(*buffer++);

            rows[y][x * COLOR_CHANNELS + 0] = (png_byte)rgb.r;
            rows[y][x * COLOR_CHANNELS + 1] = (png_byte)rgb.g;
            rows[y][x * COLOR_CHANNELS + 2] = (png_byte)rgb.b;
        }
    }

    if(Title) {
        text[numText].compression = PNG_TEXT_COMPRESSION_NONE;
        text[numText].key = "Title";
        text[numText].text = Title;
        numText++;
    }

    if(Description) {
        text[numText].compression = PNG_TEXT_COMPRESSION_NONE;
        text[numText].key = "Description";
        text[numText].text = Description;
        numText++;
    }

    png_init_io(png, f_file_handleGet(f));
    png_set_rows(png, info, rows);
    png_set_text(png, info, text, numText);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png, NULL);

cleanUp:
    if(png) {
        png_destroy_write_struct(&png, info ? &info : NULL);
    }

    f_mem_free(rows);
    f_mem_free(rowsData);

    f_file_free(f);
}
#endif // F_CONFIG_LIB_PNG
