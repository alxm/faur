/*
    Copyright 2018, 2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_BLOCK_V_H
#define F_INC_DATA_BLOCK_V_H

#include "f_block.p.h"

#include "../data/f_hash.v.h"

struct FBlock {
    char* text; // own content
    FList* blocks; // FList<FBlock*>, all blocks indented under this block
    FHash* index; // FHash<const char*, FList<const FBlock*>> indexed by text
    const FBlock** array; // the blocks indexed by line # relative to parent
    unsigned arrayLen; // number of blocks under parent
    int references; // added when merged into another parent block
};

extern void f_block__refInc(FBlock* Block);
extern void f_block__merge(FBlock* Dst, const FBlock* Src);

#endif // F_INC_DATA_BLOCK_V_H
