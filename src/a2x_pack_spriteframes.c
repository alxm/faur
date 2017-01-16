/*
    Copyright 2010, 2016 Alex Margarit

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
    unsigned num;
    unsigned int countdown;
    unsigned int callsToNextFrame;
    unsigned index;
    bool forward;
    bool paused;
};

ASpriteFrames* a_spriteframes_new(const ASprite* Sheet, int X, int Y, unsigned int CallsToNextFrame)
{
    if(CallsToNextFrame < 1) {
        a_out__fatal("a_spriteframes_new: CallsToNextFrame<1");
    }

    ASpriteFrames* const Frames = a_mem_malloc(sizeof(ASpriteFrames));

    Frames->sprites = a_list_new();
    Frames->spriteArray = NULL;
    Frames->num = 0;
    Frames->countdown = CallsToNextFrame;
    Frames->callsToNextFrame = CallsToNextFrame;
    Frames->index = 0;
    Frames->forward = true;
    Frames->paused = false;

    while(X < Sheet->w) {
        ASprite* s = a_sprite_fromSprite(Sheet, X, Y);

        a_list_addLast(Frames->sprites, s);
        X += s->w;

        if(X < Sheet->w) {
            bool end = true;

            for(int y = Y + s->h; y-- > Y; ) {
                if(a_sprite__getPixel(Sheet, X, y) != A_SPRITE_END) {
                    end = false;
                    break;
                }
            }

            if(end) {
                break;
            }
        }

        X += 1;
    }

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
    const unsigned oldindex = Frames->index;

    if(!Frames->paused) {
        if(--Frames->countdown == 0) {
            Frames->countdown = Frames->callsToNextFrame;

            if(Frames->forward && ++Frames->index == Frames->num) {
                Frames->index = 0;
            } else if(!Frames->forward && Frames->index-- == 0) {
                Frames->index = Frames->num - 1;
            }
        }
    }

    return Frames->spriteArray[oldindex];
}

ASprite* a_spriteframes_get(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[Frames->index];
}

ASprite* a_spriteframes_getByIndex(const ASpriteFrames* Frames, unsigned Index)
{
    return Frames->spriteArray[Index];
}

ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[a_random_int(Frames->num)];
}

unsigned a_spriteframes_num(const ASpriteFrames* Frames)
{
    return Frames->num;
}

unsigned a_spriteframes_currentIndex(const ASpriteFrames* Frames)
{
    return Frames->index;
}

void a_spriteframes_setDirection(ASpriteFrames* Frames, bool Forward)
{
    Frames->forward = Forward;
}

void a_spriteframes_flipDirection(ASpriteFrames* Frames)
{
    Frames->forward = !Frames->forward;
}

unsigned int a_spriteframes_getSpeed(const ASpriteFrames* Frames)
{
    return Frames->callsToNextFrame;
}

void a_spriteframes_setSpeed(ASpriteFrames* Frames, unsigned int CallsToNextFrame)
{
    if(CallsToNextFrame < 1) {
        a_out__fatal("a_spriteframes_setSpeed: CallsToNextFrame<1");
    }

    Frames->callsToNextFrame = CallsToNextFrame;
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

    if(Frames->forward) {
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
    f->forward = true;
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
