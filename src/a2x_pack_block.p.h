/*
    Copyright 2018 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

typedef struct ABlock ABlock;

#include "a2x_pack_fix.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_pixel.p.h"

extern ABlock* a_block_new(const char* File);
extern void a_block_free(ABlock* Block);

extern AList* a_block_getAll(const ABlock* Block);
extern AList* a_block_getAllFilter(const ABlock* Block, const char* Key);
extern const ABlock* a_block_get(const ABlock* Block, const char* Key);
extern bool a_block_has(const ABlock* Block, const char* Key);

extern int a_block_lineGetInt(const ABlock* Block, unsigned LineNumber);
extern unsigned a_block_lineGetIntu(const ABlock* Block, unsigned LineNumber);
extern AFix a_block_lineGetFix(const ABlock* Block, unsigned LineNumber);
extern AFixu a_block_lineGetAngle(const ABlock* Block, unsigned LineNumber);
extern APixel a_block_lineGetPixel(const ABlock* Block, unsigned LineNumber);
extern const char* a_block_lineGetString(const ABlock* Block, unsigned LineNumber);
extern AVectorInt a_block_lineGetCoords(const ABlock* Block, unsigned LineNumber);

static inline int a_block_keyGetInt(const ABlock* Block, const char* Key)
{
    return a_block_lineGetInt(a_block_get(Block, Key), 1);
}

static inline unsigned a_block_keyGetIntu(const ABlock* Block, const char* Key)
{
    return a_block_lineGetIntu(a_block_get(Block, Key), 1);
}

static inline AFix a_block_keyGetFix(const ABlock* Block, const char* Key)
{
    return a_block_lineGetFix(a_block_get(Block, Key), 1);
}

static inline AFixu a_block_keyGetAngle(const ABlock* Block, const char* Key)
{
    return a_block_lineGetAngle(a_block_get(Block, Key), 1);
}

static inline APixel a_block_keyGetPixel(const ABlock* Block, const char* Key)
{
    return a_block_lineGetPixel(a_block_get(Block, Key), 1);
}

static inline const char* a_block_keyGetString(const ABlock* Block, const char* Key)
{
    return a_block_lineGetString(a_block_get(Block, Key), 1);
}

static inline AVectorInt a_block_keyGetCoords(const ABlock* Block, const char* Key)
{
    return a_block_lineGetCoords(a_block_get(Block, Key), 1);
}
