/*
    Copyright 2010, 2016-2018 Alex Margarit

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

typedef struct ASpriteFrames ASpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern ASpriteFrames* a_spriteframes_newFromFile(const char* Path, unsigned CallsToNextFrame);
extern ASpriteFrames* a_spriteframes_newFromSprite(const ASprite* Sheet, int X, int Y, unsigned CallsToNextFrame);
extern ASpriteFrames* a_spriteframes_newBlank(unsigned CallsToNextFrame);
extern ASpriteFrames* a_spriteframes_dup(const ASpriteFrames* Frames, bool DupSprites);
extern void a_spriteframes_free(ASpriteFrames* Frames, bool FreeSprites);

extern void a_spriteframes_reset(ASpriteFrames* Frames);
extern void a_spriteframes_randomize(ASpriteFrames* Frames);

extern void a_spriteframes_push(ASpriteFrames* Frames, ASprite* Sprite);
extern ASprite* a_spriteframes_pop(ASpriteFrames* Frames);

extern ASprite* a_spriteframes_getNext(ASpriteFrames* Frames);
extern ASprite* a_spriteframes_getCurrent(const ASpriteFrames* Frames);
extern ASprite* a_spriteframes_getByIndex(const ASpriteFrames* Frames, unsigned Index);
extern ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames);

extern unsigned a_spriteframes_getNum(const ASpriteFrames* Frames);
extern unsigned a_spriteframes_getIndex(const ASpriteFrames* Frames);
extern void a_spriteframes_setIndex(ASpriteFrames* Frames, unsigned Index);

extern void a_spriteframes_setDirection(ASpriteFrames* Frames, bool Forward);
extern void a_spriteframes_flipDirection(ASpriteFrames* Frames);

extern unsigned a_spriteframes_getSpeed(const ASpriteFrames* Frames);
extern void a_spriteframes_setSpeed(ASpriteFrames* Frames, unsigned CallsToNextFrame);

extern void a_spriteframes_pause(ASpriteFrames* Frames);
extern void a_spriteframes_resume(ASpriteFrames* Frames);

extern AList* a_spriteframes_getSprites(const ASpriteFrames* Frames);
