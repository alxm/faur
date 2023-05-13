/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_BITFIELD_V_H
#define F_INC_DATA_BITFIELD_V_H

#include "f_bitfield.p.h"

typedef unsigned long FChunk;

typedef enum {
    F_BITFIELD_32,
    F_BITFIELD_64,
    F_BITFIELD_FLEX
} FBitfieldType;

struct FBitfield {
    FBitfieldType type;
    union {
        uint32_t u32;
        uint64_t u64;
        struct {
            unsigned numChunks;
            FChunk chunks[1]; // [numChunks]
        } flex;
    } u;
};

#endif // F_INC_DATA_BITFIELD_V_H
