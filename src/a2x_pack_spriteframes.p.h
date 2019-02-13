/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

typedef struct ASpriteFrames ASpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"
#include "a2x_pack_timer.p.h"

extern ASpriteFrames* a_spriteframes_newBlank(void);
extern ASpriteFrames* a_spriteframes_newFromPng(const char* Path, int CellWidth, int CellHeight);
extern ASpriteFrames* a_spriteframes_newFromSprite(const ASprite* Sheet, int X, int Y, int CellWidth, int CellHeight);
extern ASpriteFrames* a_spriteframes_dup(const ASpriteFrames* Frames, bool DupSprites);
extern void a_spriteframes_free(ASpriteFrames* Frames, bool FreeSprites);

extern void a_spriteframes_clear(ASpriteFrames* Frames, bool FreeSprites);
extern void a_spriteframes_reset(ASpriteFrames* Frames);
extern void a_spriteframes_randomize(ASpriteFrames* Frames);

extern void a_spriteframes_addFirst(ASpriteFrames* Frames, ASprite* Sprite);
extern void a_spriteframes_addLast(ASpriteFrames* Frames, ASprite* Sprite);
extern ASprite* a_spriteframes_removeFirst(ASpriteFrames* Frames);
extern ASprite* a_spriteframes_removeLast(ASpriteFrames* Frames);

extern ASprite* a_spriteframes_getNext(ASpriteFrames* Frames);
extern ASprite* a_spriteframes_getCurrent(const ASpriteFrames* Frames);
extern ASprite* a_spriteframes_getByIndex(const ASpriteFrames* Frames, unsigned Index);
extern ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames);

extern AList* a_spriteframes_framesGet(const ASpriteFrames* Frames);
extern unsigned a_spriteframes_framesGetNum(const ASpriteFrames* Frames);

extern unsigned a_spriteframes_indexGet(const ASpriteFrames* Frames);
extern void a_spriteframes_indexSet(ASpriteFrames* Frames, unsigned Index);

extern void a_spriteframes_directionSet(ASpriteFrames* Frames, bool Forward);
extern void a_spriteframes_directionFlip(ASpriteFrames* Frames);

extern unsigned a_spriteframes_speedGet(const ASpriteFrames* Frames);
extern unsigned a_spriteframes_speedGetUnitsPerCycle(const ASpriteFrames* Frames);
extern void a_spriteframes_speedSet(ASpriteFrames* Frames, ATimerType Units, unsigned TimePerFrame);

extern void a_spriteframes_start(ASpriteFrames* Frames);
extern void a_spriteframes_pause(ASpriteFrames* Frames);
