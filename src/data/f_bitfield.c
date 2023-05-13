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

    FBitfield* b;

    if(NumBits <= 64) {
        b = f_pool__alloc(F_POOL__BITFIELD);

        b->type = NumBits <= 32 ? F_BITFIELD_32 : F_BITFIELD_64;
    } else {
        unsigned num = (NumBits + F__BITS_PER_CHUNK - 1) / F__BITS_PER_CHUNK;

        b = f_mem_mallocz(sizeof(FBitfield) + (num - 1) * sizeof(FChunk));

        b->type = F_BITFIELD_FLEX;
        b->u.flex.numChunks = num;
    }

    return b;
}

void f_bitfield_free(FBitfield* Bitfield)
{
    if(Bitfield == NULL) {
        return;
    }

    if(Bitfield->type != F_BITFIELD_FLEX) {
        f_pool_release(Bitfield);
    } else {
        f_mem_free(Bitfield);
    }
}

void f_bitfield_set(FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);

    if(Bitfield->type == F_BITFIELD_32) {
        F__CHECK(Bit < 32);

        Bitfield->u.u32 |= (uint32_t)1 << Bit;
    } else if(Bitfield->type == F_BITFIELD_64) {
        F__CHECK(Bit < 64);

        Bitfield->u.u64 |= (uint64_t)1 << Bit;
    } else {
        F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->u.flex.numChunks);

        FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);
        Bitfield->u.flex.chunks[Bit / F__BITS_PER_CHUNK] |= bit;
    }
}

void f_bitfield_clear(FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);

    if(Bitfield->type == F_BITFIELD_32) {
        F__CHECK(Bit < 32);

        Bitfield->u.u32 &= ~((uint32_t)1 << Bit);
    } else if(Bitfield->type == F_BITFIELD_64) {
        F__CHECK(Bit < 64);

        Bitfield->u.u64 &= ~((uint64_t)1 << Bit);
    } else {
        F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->u.flex.numChunks);

        FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);
        Bitfield->u.flex.chunks[Bit / F__BITS_PER_CHUNK] &= ~bit;
    }
}

void f_bitfield_reset(FBitfield* Bitfield)
{
    F__CHECK(Bitfield != NULL);

    if(Bitfield->type == F_BITFIELD_32) {
        Bitfield->u.u32 = 0;
    } else if(Bitfield->type == F_BITFIELD_64) {
        Bitfield->u.u64 = 0;
    } else {
        memset(Bitfield->u.flex.chunks,
               0,
               Bitfield->u.flex.numChunks * sizeof(FChunk));
    }
}

bool f_bitfield_test(const FBitfield* Bitfield, unsigned Bit)
{
    F__CHECK(Bitfield != NULL);

    if(Bitfield->type == F_BITFIELD_32) {
        F__CHECK(Bit < 32);

        return (Bitfield->u.u32 & ((uint32_t)1 << Bit)) != 0;
    } else if(Bitfield->type == F_BITFIELD_64) {
        F__CHECK(Bit < 64);

        return (Bitfield->u.u64 & ((uint64_t)1 << Bit)) != 0;
    } else {
        F__CHECK(Bit / F__BITS_PER_CHUNK < Bitfield->u.flex.numChunks);

        FChunk value = Bitfield->u.flex.chunks[Bit / F__BITS_PER_CHUNK];
        FChunk bit = (FChunk)1 << (Bit & F__BITS_PER_CHUNK_MASK);

        return (value & bit) != 0;
    }
}

bool f_bitfield_testMask(const FBitfield* Bitfield, const FBitfield* Mask)
{
    F__CHECK(Bitfield != NULL);
    F__CHECK(Mask != NULL);
    F__CHECK(Bitfield->type == Mask->type);

    if(Bitfield->type == F_BITFIELD_32) {
        return (Bitfield->u.u32 & Mask->u.u32) == Mask->u.u32;
    } else if(Bitfield->type == F_BITFIELD_64) {
        return (Bitfield->u.u64 & Mask->u.u64) == Mask->u.u64;
    } else {
        F__CHECK(Mask->u.flex.numChunks <= Bitfield->u.flex.numChunks);

        for(unsigned i = Mask->u.flex.numChunks; i--; ) {
            if((Bitfield->u.flex.chunks[i] & Mask->u.flex.chunks[i])
                != Mask->u.flex.chunks[i]) {

                return false;
            }
        }

        return true;
    }

    return false;
}
