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

#include "a2x_pack_spriteframes.v.h"

struct ASpriteFrames {
    AList* sprites;
    ASprite** spriteArray;
    int num;
    unsigned int countdown;
    unsigned int callsToNextFrame;
    int index;
    int dir;
    bool paused;
};

ASpriteFrames* a_spriteframes_new(const ASprite* Sheet, int X, int Y, unsigned int CallsToNextFrame)
{
    ASpriteFrames* const Frames = a_mem_malloc(sizeof(ASpriteFrames));

    Frames->sprites = a_list_new();
    Frames->spriteArray = NULL;
    Frames->num = 0;
    Frames->countdown = CallsToNextFrame;
    Frames->callsToNextFrame = CallsToNextFrame;
    Frames->index = 0;
    Frames->dir = 1;
    Frames->paused = false;

    ASprite* s;

    do {
        s = a_sprite_new(Sheet, X, Y);

        if(s) {
            a_list_addLast(Frames->sprites, s);

            if(a_sprite__getPixel(Sheet, X + s->w, Y) == A_SPRITE_END) {
                s = NULL;
            } else {
                X += s->w + 1;
            }
        }
    } while(s != NULL);

    Frames->spriteArray = (ASprite**)a_list_array(Frames->sprites);
    Frames->num = a_list_size(Frames->sprites);

    return Frames;
}

void a_spriteframes_free(ASpriteFrames* Frames, bool DoFreeSprites)
{
    if(DoFreeSprites) {
        A_LIST_ITERATE(Frames->sprites, ASprite*, sprite) {
            a_sprite_free(sprite);
        }
    }

    a_list_free(Frames->sprites);
    free(Frames->spriteArray);

    free(Frames);
}

ASprite* a_spriteframes_next(ASpriteFrames* Frames)
{
    const int oldindex = Frames->index;

    if(!Frames->paused) {
        if(Frames->countdown-- == 0) {
            Frames->countdown = Frames->callsToNextFrame;
            Frames->index += Frames->dir;

            if(Frames->index < 0) {
                Frames->index = Frames->num - 1;
            } else if(Frames->index >= Frames->num) {
                Frames->index = 0;
            }
        }
    }

    return Frames->spriteArray[oldindex];
}

ASprite* a_spriteframes_get(ASpriteFrames* Frames)
{
    return Frames->spriteArray[Frames->index];
}

ASprite* a_spriteframes_geti(ASpriteFrames* Frames, int Index)
{
    return Frames->spriteArray[Index];
}

bool a_spriteframes_last(const ASpriteFrames* Frames)
{
    if(Frames->countdown == 1) {
        const int n = Frames->index + Frames->dir;
        return n < 0 || n >= Frames->num;
    }

    return false;
}

void a_spriteframes_setDirection(ASpriteFrames* Frames, int Direction)
{
    Frames->dir = Direction;
}

void a_spriteframes_flipDirection(ASpriteFrames* Frames)
{
    Frames->dir *= -1;
}

void a_spriteframes_setSpeed(ASpriteFrames* Frames, unsigned int CallsToNextFrame)
{
    Frames->callsToNextFrame = CallsToNextFrame;
    a_spriteframes_reset(Frames);
}

void a_spriteframes_pause(ASpriteFrames* Frames)
{
    Frames->paused = true;
}

void a_spriteframes_resume(ASpriteFrames* Frames)
{
    Frames->paused = false;
}

void a_spriteframes_reset(ASpriteFrames* Frames)
{
    Frames->countdown = Frames->callsToNextFrame;

    if(Frames->dir == 1) {
        Frames->index = 0;
    } else {
        Frames->index = Frames->num - 1;
    }
}

ASpriteFrames* a_spriteframes_clone(const ASpriteFrames* Frames)
{
    ASpriteFrames* const f = a_mem_malloc(sizeof(ASpriteFrames));

    f->sprites = a_list_clone(Frames->sprites);
    f->spriteArray = (ASprite**)a_list_array(Frames->sprites);
    f->num = Frames->num;
    f->countdown = Frames->callsToNextFrame;
    f->callsToNextFrame = Frames->callsToNextFrame;
    f->index = 0;
    f->dir = 1;
    f->paused = false;

    return f;
}

AList* a_spriteframes_sprites(const ASpriteFrames* Frames)
{
    return Frames->sprites;
}

ASprite* a_spriteframes_pop(ASpriteFrames* Frames)
{
    ASprite* s = a_list_pop(Frames->sprites);

    free(Frames->spriteArray);
    Frames->spriteArray = (ASprite**)a_list_array(Frames->sprites);
    Frames->num = a_list_size(Frames->sprites);

    a_spriteframes_reset(Frames);

    return s;
}
