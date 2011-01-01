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

#ifndef A2X_PACK_ANIMATION_PH
#define A2X_PACK_ANIMATION_PH

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "a2x_pack_fix.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_screen.p.h"
#include "a2x_pack_sheet.p.h"
#include "a2x_pack_sprite.p.h"

typedef struct Animation Animation;

extern Animation* a_animation_make(const int framesPerCycle);
extern Animation* a_animation_fromSheet(const Sheet* const sh, const int x, const int y, const int framesPerCycle);
extern void a_animation_free(Animation* const a);

extern void a_animation_add(Animation* const a, Sprite* const s);
extern Sprite* a_animation_remove(Animation* const a, const int index);

extern Sprite* a_animation_next(Animation* const a);
extern Sprite* a_animation_get(Animation* const a);

extern void a_animation_setDir(Animation* const a, const int dir);
extern void a_animation_flipDir(Animation* const a);

extern void a_animation_pause(Animation* const a);
extern void a_animation_resume(Animation* const a);

extern void a_animation_reset(Animation* const a);
extern int a_animation_frameIndex(const Animation* const a);

extern int a_animation_onLastFrame(Animation* const a);

extern Animation* a_animation_clone(const Animation* const src);
extern List* a_animation_sprites(const Animation* const a);

#endif // A2X_PACK_ANIMATION_PH
