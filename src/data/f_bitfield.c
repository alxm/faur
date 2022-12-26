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

#include "f_bitfield.v.h"
#include <faur.v.h>

#define F__BITS_PER_CHUNK (unsigned)(sizeof(FChunk) * 8)
#define F__BITS_PER_CHUNK_MASK (F__BITS_PER_CHUNK - 1)

FBitfield* f_bitfield_new(unsigned NumBits)
{
    F__CHECK(NumBits > 0);

    unsigned numChunks = (NumBits + F__BITS_PER_CHUNK - 1) / F__BITS_PER_CHUNK;
    FBitfield* b = f_mem_mallocz(
                    sizeof(FBitfield) + (numChunks - 1) * sizeof(FChunk));

    b->numChunks = numChunks;

    return b;
}

void f_bitfield_free(FBitfield* Bitfield)
{
    if(Bitfield == NULL) {
        return;
    }

    f_mem_free(Bitfield);
}

void f_bitfield_set(FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);
    F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->numChunks);

    FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);
    Bitfield->bits[Bit / F__BITS_PER_CHUNK] |= bit;
}

void f_bitfield_clear(FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);
    F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->numChunks);

    FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);
    Bitfield->bits[Bit / F__BITS_PER_CHUNK] &= ~bit;
}

void f_bitfield_reset(FBitfield* Bitfield)
{
    F__CHECK(Bitfield != NULL);

    memset(Bitfield->bits, 0, Bitfield->numChunks * sizeof(FChunk));
}

bool f_bitfield_test(const FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);
    F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->numChunks);

    FChunk value = Bitfield->bits[Bit / F__BITS_PER_CHUNK];
    FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);

    return (value & bit) != 0;
}

bool f_bitfield_testMask(const FBitfield* Bitfield, const FBitfield* Mask)
{
    F__CHECK(Bitfield != NULL);
    F__CHECK(Mask != NULL);
    F__CHECK(Mask->numChunks <= Bitfield->numChunks);

    for(unsigned i = Mask->numChunks; i--; ) {
        if((Bitfield->bits[i] & Mask->bits[i]) != Mask->bits[i]) {
            return false;
        }
    }

    return true;
}
