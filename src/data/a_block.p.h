/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_DATA_BLOCK_P_H
#define A_INC_DATA_BLOCK_P_H

#include "general/a_system_includes.h"

typedef struct ABlock ABlock;

#include "data/a_list.p.h"
#include "graphics/a_color.p.h"
#include "math/a_fix.p.h"

extern ABlock* a_block_new(const char* File);
extern void a_block_free(ABlock* Block);

extern const AList* a_block_blocksGet(const ABlock* Block);

extern const ABlock* a_block_keyGetBlock(const ABlock* Block, const char* Key);
extern const AList* a_block_keyGetBlocks(const ABlock* Block, const char* Key);
extern bool a_block_keyExists(const ABlock* Block, const char* Key);

extern int a_block_lineGetInt(const ABlock* Block, unsigned LineNumber);
extern unsigned a_block_lineGetIntu(const ABlock* Block, unsigned LineNumber);
extern AFix a_block_lineGetFix(const ABlock* Block, unsigned LineNumber);
extern AFixu a_block_lineGetAngle(const ABlock* Block, unsigned LineNumber);
extern APixel a_block_lineGetPixel(const ABlock* Block, unsigned LineNumber);
extern const char* a_block_lineGetString(const ABlock* Block, unsigned LineNumber);
extern AVectorInt a_block_lineGetCoords(const ABlock* Block, unsigned LineNumber);
extern int a_block_lineGetFmt(const ABlock* Block, unsigned LineNumber, const char* Format, ...);
extern int a_block_lineGetFmtv(const ABlock* Block, unsigned LineNumber, const char* Format, va_list Args);

static inline int a_block_keyGetInt(const ABlock* Block, const char* Key)
{
    return a_block_lineGetInt(a_block_keyGetBlock(Block, Key), 1);
}

static inline unsigned a_block_keyGetIntu(const ABlock* Block, const char* Key)
{
    return a_block_lineGetIntu(a_block_keyGetBlock(Block, Key), 1);
}

static inline AFix a_block_keyGetFix(const ABlock* Block, const char* Key)
{
    return a_block_lineGetFix(a_block_keyGetBlock(Block, Key), 1);
}

static inline AFixu a_block_keyGetAngle(const ABlock* Block, const char* Key)
{
    return a_block_lineGetAngle(a_block_keyGetBlock(Block, Key), 1);
}

static inline APixel a_block_keyGetPixel(const ABlock* Block, const char* Key)
{
    return a_block_lineGetPixel(a_block_keyGetBlock(Block, Key), 1);
}

static inline const char* a_block_keyGetString(const ABlock* Block, const char* Key)
{
    return a_block_lineGetString(a_block_keyGetBlock(Block, Key), 1);
}

static inline AVectorInt a_block_keyGetCoords(const ABlock* Block, const char* Key)
{
    return a_block_lineGetCoords(a_block_keyGetBlock(Block, Key), 1);
}

extern int a_block_keyGetFmt(const ABlock* Block, const char* Key, const char* Format, ...);

static inline int a_block_keyGetFmtv(const ABlock* Block, const char* Key, const char* Format, va_list Args)
{
    return a_block_lineGetFmtv(a_block_keyGetBlock(Block, Key), 1, Format, Args);
}

#endif // A_INC_DATA_BLOCK_P_H
