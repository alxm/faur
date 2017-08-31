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
    unsigned countdown;
    unsigned callsToNextFrame;
    unsigned index;
    bool forward;
    bool paused;
};

ASpriteFrames* a_spriteframes_new(const ASprite* Sheet, int X, int Y, unsigned CallsToNextFrame)
{
    ASpriteFrames* f = a_spriteframes_newBlank(CallsToNextFrame);

    if(X < 0 || X >= Sheet->w || Y < 0 || Y >= Sheet->h) {
        a_out__fatal("%s coords %d, %d are invalid",
                     A_SPRITE__NAME(Sheet),
                     X,
                     Y);
    }

    while(X < Sheet->w) {
        ASprite* s = a_sprite_newFromSprite(Sheet, X, Y);

        a_list_addLast(f->sprites, s);
        X += s->w;

        if(X < Sheet->w) {
            bool end = true;

            for(int y = Y + s->h; y-- > Y; ) {
                if(a_sprite__getPixel(Sheet, X, y) != a_sprite__colorEnd) {
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

    f->spriteArray = (ASprite**)a_list_toArray(f->sprites);
    f->num = a_list_getSize(f->sprites);

    return f;
}

ASpriteFrames* a_spriteframes_newBlank(unsigned CallsToNextFrame)
{
    if(CallsToNextFrame < 1) {
        a_out__fatal("a_spriteframes_newBlank: CallsToNextFrame<1");
    }

    ASpriteFrames* f = a_mem_malloc(sizeof(ASpriteFrames));

    f->sprites = a_list_new();
    f->spriteArray = NULL;
    f->num = 0;
    f->countdown = CallsToNextFrame;
    f->callsToNextFrame = CallsToNextFrame;
    f->index = 0;
    f->forward = true;
    f->paused = false;

    return f;
}

ASpriteFrames* a_spriteframes_dup(const ASpriteFrames* Frames)
{
    ASpriteFrames* f = a_mem_malloc(sizeof(ASpriteFrames));

    f->sprites = a_list_dup(Frames->sprites);
    f->spriteArray = (ASprite**)a_list_toArray(Frames->sprites);
    f->num = Frames->num;
    f->countdown = Frames->callsToNextFrame;
    f->callsToNextFrame = Frames->callsToNextFrame;
    f->index = 0;
    f->forward = true;
    f->paused = false;

    return f;
}

void a_spriteframes_free(ASpriteFrames* Frames, bool FreeSprites)
{
    if(FreeSprites) {
        a_list_freeEx(Frames->sprites, (AFree*)a_sprite_free);
    } else {
        a_list_free(Frames->sprites);
    }

    free(Frames->spriteArray);
    free(Frames);
}

void a_spriteframes_push(ASpriteFrames* Frames, ASprite* Sprite)
{
    a_list_push(Frames->sprites, Sprite);
    Frames->num++;

    free(Frames->spriteArray);
    Frames->spriteArray = (ASprite**)a_list_toArray(Frames->sprites);

    a_spriteframes_reset(Frames);
}

ASprite* a_spriteframes_pop(ASpriteFrames* Frames)
{
    ASprite* s = a_list_pop(Frames->sprites);
    Frames->num--;

    free(Frames->spriteArray);
    Frames->spriteArray = (ASprite**)a_list_toArray(Frames->sprites);

    a_spriteframes_reset(Frames);

    return s;
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

ASprite* a_spriteframes_getCurrent(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[Frames->index];
}

ASprite* a_spriteframes_getIndex(const ASpriteFrames* Frames, unsigned Index)
{
    return Frames->spriteArray[Index];
}

ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[a_random_getIntu(Frames->num)];
}

unsigned a_spriteframes_getNum(const ASpriteFrames* Frames)
{
    return Frames->num;
}

unsigned a_spriteframes_getCurrentIndex(const ASpriteFrames* Frames)
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

unsigned a_spriteframes_getSpeed(const ASpriteFrames* Frames)
{
    return Frames->callsToNextFrame;
}

void a_spriteframes_setSpeed(ASpriteFrames* Frames, unsigned CallsToNextFrame)
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

AList* a_spriteframes_getSprites(const ASpriteFrames* Frames)
{
    return Frames->sprites;
}
