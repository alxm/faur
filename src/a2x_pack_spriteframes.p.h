/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_SPRITEFRAMES_PH
#define A2X_PACK_SPRITEFRAMES_PH

#include "a2x_app_includes.h"

typedef struct SpriteFrames SpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sheet.p.h"
#include "a2x_pack_sprite.p.h"

extern SpriteFrames* a_spriteframes_new(const int framesPerCycle);
extern SpriteFrames* a_spriteframes_fromSheet(const Sheet* const sh, const int x, const int y, const int framesPerCycle);
extern void a_spriteframes_free(SpriteFrames* const sf);

extern void a_spriteframes_add(SpriteFrames* const sf, Sprite* const s);
extern Sprite* a_spriteframes_remove(SpriteFrames* const sf, const int index);

extern Sprite* a_spriteframes_next(SpriteFrames* const sf);
extern Sprite* a_spriteframes_get(SpriteFrames* const sf);

extern void a_spriteframes_setDir(SpriteFrames* const sf, const int dir);
extern void a_spriteframes_flipDir(SpriteFrames* const sf);

extern void a_spriteframes_pause(SpriteFrames* const sf);
extern void a_spriteframes_resume(SpriteFrames* const sf);

extern void a_spriteframes_reset(SpriteFrames* const sf);
extern int a_spriteframes_frameIndex(const SpriteFrames* const sf);

extern bool a_spriteframes_onLastFrame(SpriteFrames* const sf);

extern SpriteFrames* a_spriteframes_clone(const SpriteFrames* const src);
extern List* a_spriteframes_sprites(const SpriteFrames* const sf);

#endif // A2X_PACK_SPRITEFRAMES_PH
