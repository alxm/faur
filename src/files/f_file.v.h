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

#ifndef F_INC_FILES_FILE_V_H
#define F_INC_FILES_FILE_V_H

#include "f_file.p.h"

typedef enum {
    F_FILE__OFFSET_INVALID = -1,
    F_FILE__OFFSET_START,
    F_FILE__OFFSET_CURRENT,
    F_FILE__OFFSET_END,
    F_FILE__OFFSET_NUM
} FFileOffset;

#include "../files/f_embed.v.h"
#include "../platform/f_platform.v.h"

struct FFile {
    FPath* path;
    union {
        void* file;
        FPlatformFile* platform;
        FFileEmbedded* embedded;
    } f;
    char* lineBuffer;
    unsigned lineBufferSize;
    unsigned lineNumber;
    bool eof;
};

#endif // F_INC_FILES_FILE_V_H
