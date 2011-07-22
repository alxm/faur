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
    int frame;
    int framesPerCycle;
    int current;
    int dir;
    int paused;
};

SpriteFrames* a_spriteframes_make(const int framesPerCycle)
{
    SpriteFrames* const a = malloc(sizeof(SpriteFrames));

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

SpriteFrames* a_spriteframes_fromSheet(const Sheet* const sh, const int x, const int y, const int framesPerCycle)
{
    SpriteFrames* const sf = a_spriteframes_make(framesPerCycle);

    const int width = sh->w;
    const int height = sh->h;

    const Pixel limit = sh->limit;
    const Pixel end = sh->end;

    int last_sheetx = x;

    for(int sheetx = x; sheetx < width; sheetx++) {
        const Pixel horizPixel = a_sheet__getPixel(sh, sheetx, y);

        // reached right edge
        if(horizPixel == limit || horizPixel == end) {
            for(int sheety = y; sheety < height; sheety++) {
                const Pixel vertPixel = a_sheet__getPixel(sh, last_sheetx, sheety);

                // reached bottom edge
                if(vertPixel == limit || vertPixel == end) {
                    const int w = sheetx - last_sheetx;
                    const int h = sheety - y;

                    Sprite* const sprite = a_sprite_make(sh, last_sheetx, y, w, h);
                    a_list_addLast(sf->sprites, sprite);

                    break;
                }
            }

            last_sheetx = sheetx + 1;

            if(horizPixel == end) {
                break;
            }
        }
    }

    sf->spriteArray = (Sprite**)a_list_getArray(sf->sprites);
    sf->num = a_list_size(sf->sprites);

    return sf;
}

void a_spriteframes_free(SpriteFrames* const sf)
{
    a_list_free(sf->sprites);
    free(sf->spriteArray);

    free(sf);
}

void a_spriteframes_add(SpriteFrames* const sf, Sprite* const s)
{
    a_list_addLast(sf->sprites, s);

    free(sf->spriteArray);
    sf->spriteArray = (Sprite**)a_list_getArray(sf->sprites);

    sf->num++;

    a_spriteframes_reset(sf);
}

Sprite* a_spriteframes_remove(SpriteFrames* const sf, const int index)
{
    Sprite* const s = sf->spriteArray[index];

    a_list_remove(sf->sprites, s);

    free(sf->spriteArray);
    sf->spriteArray = (Sprite**)a_list_getArray(sf->sprites);

    sf->num--;

    a_spriteframes_reset(sf);

    return s;
}

Sprite* a_spriteframes_next(SpriteFrames* const sf)
{
    if(!sf->paused) {
        sf->frame += sf->num;

        if(sf->frame >= sf->framesPerCycle) {
            sf->frame -= sf->framesPerCycle;
            sf->current += sf->dir;

            if(sf->current < 0) {
                sf->current = sf->num - 1;
            } else if(sf->current >= sf->num) {
                sf->current = 0;
            }
        }
    }

    return sf->spriteArray[sf->current];
}

Sprite* a_spriteframes_get(SpriteFrames* const sf)
{
    return sf->spriteArray[sf->current];
}

void a_spriteframes_setDir(SpriteFrames* const sf, const int dir)
{
    sf->dir = dir;
}

void a_spriteframes_flipDir(SpriteFrames* const sf)
{
    sf->dir *= -1;
}

void a_spriteframes_pause(SpriteFrames* const sf)
{
    sf->paused = 1;
}

void a_spriteframes_resume(SpriteFrames* const sf)
{
    sf->paused = 0;
}

void a_spriteframes_reset(SpriteFrames* const sf)
{
    sf->frame = 0;

    if(sf->dir == 1) {
        sf->current = 0;
    } else {
        sf->current = sf->num - 1;
    }
}

int a_spriteframes_frameIndex(const SpriteFrames* const sf)
{
    return sf->current;
}

bool a_spriteframes_onLastFrame(SpriteFrames* const sf)
{
    if(sf->frame + sf->num >= sf->framesPerCycle) {
        const int n = sf->current + sf->dir;
        return n < 0 || n >= sf->num;
    }

    return false;
}

SpriteFrames* a_spriteframes_clone(const SpriteFrames* const src)
{
    SpriteFrames* const sf = a_spriteframes_make(src->framesPerCycle);

    while(a_list_iterate(src->sprites)) {
        a_spriteframes_add(sf, a_list_current(src->sprites));
    }

    return sf;
}

List* a_spriteframes_sprites(const SpriteFrames* const sf)
{
    return sf->sprites;
}
