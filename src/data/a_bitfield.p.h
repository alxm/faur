/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_DATA_BITFIELD_P_H
#define A_INC_DATA_BITFIELD_P_H

#include "general/a_system_includes.h"

typedef struct ABitfield ABitfield;

extern ABitfield* a_bitfield_new(unsigned NumBits);
extern void a_bitfield_free(ABitfield* Bitfield);

extern void a_bitfield_set(ABitfield* Bitfield, unsigned Bit);
extern void a_bitfield_clear(ABitfield* Bitfield, unsigned Bit);
extern void a_bitfield_reset(ABitfield* Bitfield);

extern bool a_bitfield_test(const ABitfield* Bitfield, unsigned Bit);
extern bool a_bitfield_testMask(const ABitfield* Bitfield, const ABitfield* Mask);

#endif // A_INC_DATA_BITFIELD_P_H
