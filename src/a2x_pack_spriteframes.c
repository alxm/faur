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

ASpriteFrames* a_spriteframes_new(const ASprite* sheet, int x, int y, unsigned int callsToNextFrame)
{
    ASpriteFrames* const sf = a_mem_malloc(sizeof(ASpriteFrames));

    sf->sprites = a_list_new();
    sf->spriteArray = NULL;
    sf->num = 0;
    sf->countdown = callsToNextFrame;
    sf->callsToNextFrame = callsToNextFrame;
    sf->index = 0;
    sf->dir = 1;
    sf->paused = false;

    ASprite* s;

    do {
        s = a_sprite_new(sheet, x, y);

        if(s) {
            a_list_addLast(sf->sprites, s);

            if(a_sprite__getPixel(sheet, x + s->w, y) == A_SPRITE_END) {
                s = NULL;
            } else {
                x += s->w + 1;
            }
        }
    } while(s != NULL);

    sf->spriteArray = (ASprite**)a_list_array(sf->sprites);
    sf->num = a_list_size(sf->sprites);

    return sf;
}

void a_spriteframes_free(ASpriteFrames* sf, bool freeSprites)
{
    if(freeSprites) {
        A_LIST_ITERATE(sf->sprites, ASprite, sprite) {
            a_sprite_free(sprite);
        }
    }

    a_list_free(sf->sprites);
    free(sf->spriteArray);

    free(sf);
}

ASprite* a_spriteframes_next(ASpriteFrames* sf)
{
    const int oldindex = sf->index;

    if(!sf->paused) {
        if(sf->countdown-- == 0) {
            sf->countdown = sf->callsToNextFrame;
            sf->index += sf->dir;

            if(sf->index < 0) {
                sf->index = sf->num - 1;
            } else if(sf->index >= sf->num) {
                sf->index = 0;
            }
        }
    }

    return sf->spriteArray[oldindex];
}

ASprite* a_spriteframes_get(ASpriteFrames* sf)
{
    return sf->spriteArray[sf->index];
}

ASprite* a_spriteframes_geti(ASpriteFrames* sf, int index)
{
    return sf->spriteArray[index];
}

bool a_spriteframes_last(const ASpriteFrames* sf)
{
    if(sf->countdown == 1) {
        const int n = sf->index + sf->dir;
        return n < 0 || n >= sf->num;
    }

    return false;
}

void a_spriteframes_setDir(ASpriteFrames* sf, int dir)
{
    sf->dir = dir;
}

void a_spriteframes_flipDir(ASpriteFrames* sf)
{
    sf->dir *= -1;
}

void a_spriteframes_setSpeed(ASpriteFrames* sf, unsigned int callsToNextFrame)
{
    sf->callsToNextFrame = callsToNextFrame;
    a_spriteframes_reset(sf);
}

void a_spriteframes_pause(ASpriteFrames* sf)
{
    sf->paused = true;
}

void a_spriteframes_resume(ASpriteFrames* sf)
{
    sf->paused = false;
}

void a_spriteframes_reset(ASpriteFrames* sf)
{
    sf->countdown = sf->callsToNextFrame;

    if(sf->dir == 1) {
        sf->index = 0;
    } else {
        sf->index = sf->num - 1;
    }
}

ASpriteFrames* a_spriteframes_clone(const ASpriteFrames* src)
{
    ASpriteFrames* const sf = a_mem_malloc(sizeof(ASpriteFrames));

    sf->sprites = a_list_clone(src->sprites);
    sf->spriteArray = (ASprite**)a_list_array(sf->sprites);
    sf->num = src->num;
    sf->countdown = src->callsToNextFrame;
    sf->callsToNextFrame = src->callsToNextFrame;
    sf->index = 0;
    sf->dir = 1;
    sf->paused = false;

    return sf;
}

AList* a_spriteframes_sprites(const ASpriteFrames* sf)
{
    return sf->sprites;
}

ASprite* a_spriteframes_pop(ASpriteFrames* sf)
{
    ASprite* s = a_list_pop(sf->sprites);

    free(sf->spriteArray);
    sf->spriteArray = (ASprite**)a_list_array(sf->sprites);
    sf->num = a_list_size(sf->sprites);

    a_spriteframes_reset(sf);

    return s;
}
