/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_file.v.h"

#include "a2x_pack_file_embedded.v.h"
#include "a2x_pack_file_real.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

AFile* a_file_new(const char* Path, AFileMode Mode)
{
    AFile* f = NULL;
    APath* path = a_path_new(Path);

    if(A_FLAG_TEST_ANY(Mode, A_FILE_WRITE)
        || a_path_test(path, A_PATH_FILE | A_PATH_REAL)) {

        f = a_file_real__new(path, Mode);
    } else if(a_path_test(path, A_PATH_FILE | A_PATH_EMBEDDED)) {
        f = a_file_embedded__new(path);
    }

    if(f == NULL) {
        a_path_free(path);
    }

    return f;
}

void a_file_free(AFile* File)
{
    if(File == NULL) {
        return;
    }

    if(a_path_test(File->path, A_PATH_REAL)) {
        fclose(File->u.handle);
    }

    a_path_free(File->path);

    free(File->lineBuffer);
    free(File);
}

const APath* a_file_pathGet(const AFile* File)
{
    return File->path;
}

FILE* a_file_handleGet(const AFile* File)
{
    if(a_path_test(File->path, A_PATH_REAL)) {
        return File->u.handle;
    } else {
        return NULL;
    }
}

const AEmbeddedFile* a_file__dataGet(AFile* File)
{
    if(a_path_test(File->path, A_PATH_EMBEDDED)) {
        return File->u.e.data;
    } else {
        return NULL;
    }
}

uint8_t* a_file_toBuffer(const char* Path)
{
    if(a_path_exists(Path, A_PATH_FILE | A_PATH_REAL)) {
        return a_file_real__toBuffer(Path);
    } else if(a_path_exists(Path, A_PATH_FILE | A_PATH_EMBEDDED)) {
        return a_file_embedded__toBuffer(Path);
    }

    return NULL;
}

bool a_file_prefixCheck(AFile* File, const char* Prefix)
{
    size_t size = strlen(Prefix) + 1;
    char buffer[size];

    a_file_seekStart(File, 0);

    if(!a_file_read(File, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return a_str_equal(buffer, Prefix);
}

void a_file_prefixWrite(AFile* File, const char* Prefix)
{
    a_file_write(File, Prefix, strlen(Prefix) + 1);
}

bool a_file_read(AFile* File, void* Buffer, size_t Size)
{
    bool ret = File->interface->read(File, Buffer, Size);

    if(!ret) {
        a_out__warning("a_file_read(%s): Could not read %u bytes",
                       a_path_getFull(File->path),
                       Size);
    }

    return ret;
}

bool a_file_write(AFile* File, const void* Buffer, size_t Size)
{
    bool ret = File->interface->write(File, Buffer, Size);

    if(!ret) {
        a_out__error("a_file_write(%s): Could not write %u bytes",
                     a_path_getFull(File->path),
                     Size);
    }

    return ret;
}

bool a_file_writef(AFile* File, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    bool ret = File->interface->writef(File, Format, args);

    va_end(args);

    if(!ret) {
        a_out__error(
            "a_file_writef(%s): Could not write", a_path_getFull(File->path));
    }

    return ret;
}

bool a_file_flush(AFile* File)
{
    return File->interface->flush(File);
}

static int readChar(AFile* File)
{
    int ch = File->interface->getchar(File);

    if(ch == '\r') {
        // Check if \r is followed by \n for CRLF line endings
        ch = File->interface->getchar(File);

        if(ch != '\n') {
            // \r not followed by \n, assume CR line endings and put char back
            ch = File->interface->ungetchar(File, ch);

            if(ch != EOF) {
                File->lineNumber++;
                ch = '\r';
            }
        }
    }

    if(ch == '\n') {
        // Sequence was \n or \r\n
        File->lineNumber++;
    } else if(ch == EOF) {
        File->eof = true;
    }

    return ch;
}

bool a_file_lineRead(AFile* File)
{
    int ch;

    do {
        ch = readChar(File);
    } while(ch == '\n' || ch == '\r');

    if(File->eof) {
        return false;
    }

    unsigned index = 0;

    do {
        if(index + 1 >= File->lineBufferSize) {
            unsigned newSize = a_math_maxu(File->lineBufferSize * 2, 64);
            char* newBuffer = a_mem_malloc(newSize);

            if(File->lineBufferSize > 0) {
                memcpy(newBuffer, File->lineBuffer, File->lineBufferSize);
            }

            free(File->lineBuffer);

            File->lineBuffer = newBuffer;
            File->lineBufferSize = newSize;
        }

        File->lineBuffer[index++] = (char)ch;
        ch = readChar(File);
    } while(!File->eof && ch != '\n' && ch != '\r');

    File->lineBuffer[index] = '\0';

    return true;
}

const char* a_file_lineBufferGet(const AFile* File)
{
    return File->lineBuffer;
}

unsigned a_file_lineNumberGet(const AFile* File)
{
    return File->lineNumber;
}

bool a_file_rewind(AFile* File)
{
    bool ret = File->interface->seek(File, 0, A_FILE__OFFSET_START);

    if(ret) {
        File->lineNumber = 0;
        File->eof = false;
    } else {
        a_out__error("a_file_rewind(%s) failed", a_path_getFull(File->path));
    }

    return ret;
}

bool a_file_seekStart(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, A_FILE__OFFSET_START);

    if(!ret) {
        a_out__error("a_file_seekStart(%s, %d) failed",
                     a_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool a_file_seekEnd(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, A_FILE__OFFSET_END);

    if(!ret) {
        a_out__error("a_file_seekEnd(%s, %d) failed",
                     a_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool a_file_seekCurrent(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, A_FILE__OFFSET_CURRENT);

    if(!ret) {
        a_out__error("a_file_seekCurrent(%s, %d) failed",
                     a_path_getFull(File->path),
                     Offset);
    }

    return ret;
}
