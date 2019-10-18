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

#ifndef F_INC_DATA_BITFIELD_P_H
#define F_INC_DATA_BITFIELD_P_H

#include "general/f_system_includes.h"

typedef struct ABitfield ABitfield;

extern ABitfield* f_bitfield_new(unsigned NumBits);
extern void f_bitfield_free(ABitfield* Bitfield);

extern void f_bitfield_set(ABitfield* Bitfield, unsigned Bit);
extern void f_bitfield_clear(ABitfield* Bitfield, unsigned Bit);
extern void f_bitfield_reset(ABitfield* Bitfield);

extern bool f_bitfield_test(const ABitfield* Bitfield, unsigned Bit);
extern bool f_bitfield_testMask(const ABitfield* Bitfield, const ABitfield* Mask);

#endif // F_INC_DATA_BITFIELD_P_H
