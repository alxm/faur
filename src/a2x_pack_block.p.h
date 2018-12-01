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

extern int a_block_readInt(const ABlock* Block, unsigned LineNumber);
extern unsigned a_block_readIntu(const ABlock* Block, unsigned LineNumber);
extern AFix a_block_readFix(const ABlock* Block, unsigned LineNumber);
extern AFixu a_block_readAngle(const ABlock* Block, unsigned LineNumber);
extern APixel a_block_readPixel(const ABlock* Block, unsigned LineNumber);
extern const char* a_block_readString(const ABlock* Block, unsigned LineNumber);
extern AVectorInt a_block_readCoords(const ABlock* Block, unsigned LineNumber);
