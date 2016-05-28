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

struct SpriteFrames {
    List* sprites;
    Sprite** spriteArray;
    int num;
    unsigned int countdown;
    unsigned int callsToNextFrame;
    int index;
    int dir;
    bool paused;
};

SpriteFrames* a_spriteframes_new(const Sprite* sheet, int x, int y, unsigned int callsToNextFrame)
{
    SpriteFrames* const sf = a_mem_malloc(sizeof(SpriteFrames));

    sf->sprites = a_list_new();
    sf->spriteArray = NULL;
    sf->num = 0;
    sf->countdown = callsToNextFrame;
    sf->callsToNextFrame = callsToNextFrame;
    sf->index = 0;
    sf->dir = 1;
    sf->paused = false;

    Sprite* s;

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

    sf->spriteArray = (Sprite**)a_list_array(sf->sprites);
    sf->num = a_list_size(sf->sprites);

    return sf;
}

void a_spriteframes_free(SpriteFrames* sf)
{
    a_list_free(sf->sprites);
    free(sf->spriteArray);

    free(sf);
}

Sprite* a_spriteframes_next(SpriteFrames* sf)
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

Sprite* a_spriteframes_get(SpriteFrames* sf)
{
    return sf->spriteArray[sf->index];
}

Sprite* a_spriteframes_geti(SpriteFrames* sf, int index)
{
    return sf->spriteArray[index];
}

bool a_spriteframes_last(const SpriteFrames* sf)
{
    if(sf->countdown == 1) {
        const int n = sf->index + sf->dir;
        return n < 0 || n >= sf->num;
    }

    return false;
}

void a_spriteframes_setDir(SpriteFrames* sf, int dir)
{
    sf->dir = dir;
}

void a_spriteframes_flipDir(SpriteFrames* sf)
{
    sf->dir *= -1;
}

void a_spriteframes_pause(SpriteFrames* sf)
{
    sf->paused = true;
}

void a_spriteframes_resume(SpriteFrames* sf)
{
    sf->paused = false;
}

void a_spriteframes_reset(SpriteFrames* sf)
{
    sf->countdown = sf->callsToNextFrame;

    if(sf->dir == 1) {
        sf->index = 0;
    } else {
        sf->index = sf->num - 1;
    }
}

SpriteFrames* a_spriteframes_clone(const SpriteFrames* src)
{
    SpriteFrames* const sf = a_mem_malloc(sizeof(SpriteFrames));

    sf->sprites = a_list_clone(src->sprites);
    sf->spriteArray = (Sprite**)a_list_array(sf->sprites);
    sf->num = src->num;
    sf->countdown = src->callsToNextFrame;
    sf->callsToNextFrame = src->callsToNextFrame;
    sf->index = 0;
    sf->dir = 1;
    sf->paused = false;

    return sf;
}

List* a_spriteframes_sprites(const SpriteFrames* sf)
{
    return sf->sprites;
}

Sprite* a_spriteframes_pop(SpriteFrames* sf)
{
    Sprite* s = a_list_pop(sf->sprites);

    free(sf->spriteArray);
    sf->spriteArray = (Sprite**)a_list_array(sf->sprites);
    sf->num = a_list_size(sf->sprites);

    a_spriteframes_reset(sf);

    return s;
}
