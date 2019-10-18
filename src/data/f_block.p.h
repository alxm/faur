/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_BLOCK_P_H
#define F_INC_DATA_BLOCK_P_H

#include "general/f_system_includes.h"

typedef struct ABlock ABlock;

#include "data/f_list.p.h"
#include "graphics/f_color.p.h"
#include "math/f_fix.p.h"

extern ABlock* f_block_new(const char* File);
extern void f_block_free(ABlock* Block);

extern const AList* f_block_blocksGet(const ABlock* Block);

extern const ABlock* f_block_keyGetBlock(const ABlock* Block, const char* Key);
extern const AList* f_block_keyGetBlocks(const ABlock* Block, const char* Key);
extern bool f_block_keyExists(const ABlock* Block, const char* Key);

extern int f_block_lineGetInt(const ABlock* Block, unsigned LineNumber);
extern unsigned f_block_lineGetIntu(const ABlock* Block, unsigned LineNumber);
extern AFix f_block_lineGetFix(const ABlock* Block, unsigned LineNumber);
extern AFixu f_block_lineGetAngle(const ABlock* Block, unsigned LineNumber);
extern APixel f_block_lineGetPixel(const ABlock* Block, unsigned LineNumber);
extern const char* f_block_lineGetString(const ABlock* Block, unsigned LineNumber);
extern AVectorInt f_block_lineGetCoords(const ABlock* Block, unsigned LineNumber);
extern AVectorFix f_block_lineGetCoordsf(const ABlock* Block, unsigned LineNumber);
extern int f_block_lineGetFmt(const ABlock* Block, unsigned LineNumber, const char* Format, ...);
extern int f_block_lineGetFmtv(const ABlock* Block, unsigned LineNumber, const char* Format, va_list Args);

static inline int f_block_keyGetInt(const ABlock* Block, const char* Key)
{
    return f_block_lineGetInt(f_block_keyGetBlock(Block, Key), 1);
}

static inline unsigned f_block_keyGetIntu(const ABlock* Block, const char* Key)
{
    return f_block_lineGetIntu(f_block_keyGetBlock(Block, Key), 1);
}

static inline AFix f_block_keyGetFix(const ABlock* Block, const char* Key)
{
    return f_block_lineGetFix(f_block_keyGetBlock(Block, Key), 1);
}

static inline AFixu f_block_keyGetAngle(const ABlock* Block, const char* Key)
{
    return f_block_lineGetAngle(f_block_keyGetBlock(Block, Key), 1);
}

static inline APixel f_block_keyGetPixel(const ABlock* Block, const char* Key)
{
    return f_block_lineGetPixel(f_block_keyGetBlock(Block, Key), 1);
}

static inline const char* f_block_keyGetString(const ABlock* Block, const char* Key)
{
    return f_block_lineGetString(f_block_keyGetBlock(Block, Key), 1);
}

static inline AVectorInt f_block_keyGetCoords(const ABlock* Block, const char* Key)
{
    return f_block_lineGetCoords(f_block_keyGetBlock(Block, Key), 1);
}

static inline AVectorFix f_block_keyGetCoordsf(const ABlock* Block, const char* Key)
{
    return f_block_lineGetCoordsf(f_block_keyGetBlock(Block, Key), 1);
}

extern int f_block_keyGetFmt(const ABlock* Block, const char* Key, const char* Format, ...);

static inline int f_block_keyGetFmtv(const ABlock* Block, const char* Key, const char* Format, va_list Args)
{
    return f_block_lineGetFmtv(f_block_keyGetBlock(Block, Key), 1, Format, Args);
}

#endif // F_INC_DATA_BLOCK_P_H
