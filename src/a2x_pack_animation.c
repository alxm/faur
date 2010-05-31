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

#include "a2x_pack_animation.p.h"
#include "a2x_pack_animation.v.h"

struct Animation {
    List* sprites;
    Sprite** spriteArray;
    int num;
    int frame;
    int framesPerCycle;
    int current;
    int dir;
    int paused;
};

Animation* a_animation_make(const int framesPerCycle)
{
    Animation* const a = malloc(sizeof(Animation));

    a->sprites = a_list_set();
    a->spriteArray = NULL;

    a->num = 0;
    a->frame = 0;
    a->framesPerCycle = framesPerCycle;

    a->current = 0;
    a->dir = 1;

    a->paused = 0;

    return a;
}

void a_animation_free(Animation* const a)
{
    a_list_free(a->sprites);

    free(a);
}

void a_animation_add(Animation* const a, Sprite* const s)
{
    a_list_addLast(a->sprites, s);

    free(a->spriteArray);
    a->spriteArray = (Sprite**)a_list_getArray(a->sprites);

    a->num++;
}

Sprite* a_animation_next(Animation* const a)
{
    if(!a->paused) {
        a->frame += a->num;

        if(a->frame >= a->framesPerCycle) {
            a->frame -= a->framesPerCycle;
            a->current += a->dir;

            if(a->current < 0) {
                a->current = a->num - 1;
            } else if(a->current >= a->num) {
                a->current = 0;
            }
        }
    }

    return a->spriteArray[a->current];
}

Sprite* a_animation_get(Animation* const a)
{
    return a->spriteArray[a->current];
}

void a_animation_setDir(Animation* const a, const int dir)
{
    a->dir = dir;
}

void a_animation_flipDir(Animation* const a)
{
    a->dir *= -1;
}

void a_animation_pause(Animation* const a)
{
    a->paused = 1;
}

void a_animation_resume(Animation* const a)
{
    a->paused = 0;
}

void a_animation_reset(Animation* const a)
{
    a->frame = 0;
    a->current = 0;
}

int a_animation_frameIndex(const Animation* const a)
{
    return a->current;
}

Animation* a_animation_clone(const Animation* const src)
{
    Animation* const a = a_animation_make(src->framesPerCycle);

    while(a_list_iterate(src->sprites)) {
        a_animation_add(a, a_list_current(src->sprites));
    }

    return a;
}
