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
    int index;
    int dir;
    bool paused;
};

SpriteFrames* a_spriteframes_new(const Sprite* sheet, int x, int y, int framesPerCycle)
{
    SpriteFrames* const sf = malloc(sizeof(SpriteFrames));

    sf->sprites = a_list_new();
    sf->spriteArray = NULL;
    sf->num = 0;
    sf->frame = 0;
    sf->framesPerCycle = framesPerCycle;
    sf->index = 0;
    sf->dir = 1;
    sf->paused = false;

    const int width = sheet->w;
    const int height = sheet->h;

    int last_sheetx = x;

    for(int sheetx = x; sheetx < width; sheetx++) {
        const Pixel horizPixel = a_sprite__getPixel(sheet, sheetx, y);

        // reached right edge
        if(horizPixel == A_SPRITE_LIMIT || horizPixel == A_SPRITE_END) {
            for(int sheety = y; sheety < height; sheety++) {
                const Pixel vertPixel = a_sprite__getPixel(sheet, last_sheetx, sheety);

                // reached bottom edge
                if(vertPixel == A_SPRITE_LIMIT || vertPixel == A_SPRITE_END) {
                    const int w = sheetx - last_sheetx;
                    const int h = sheety - y;

                    Sprite* const sprite = a_sprite_new(sheet, last_sheetx, y, w, h);
                    a_list_addLast(sf->sprites, sprite);

                    break;
                }
            }

            last_sheetx = sheetx + 1;

            if(horizPixel == A_SPRITE_END) {
                break;
            }
        }
    }

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
    if(!sf->paused) {
        sf->frame += sf->num;

        if(sf->frame >= sf->framesPerCycle) {
            sf->frame -= sf->framesPerCycle;
            sf->index += sf->dir;

            if(sf->index < 0) {
                sf->index = sf->num - 1;
            } else if(sf->index >= sf->num) {
                sf->index = 0;
            }
        }
    }

    return sf->spriteArray[sf->index];
}

Sprite* a_spriteframes_get(SpriteFrames* sf)
{
    return sf->spriteArray[sf->index];
}

bool a_spriteframes_last(const SpriteFrames* sf)
{
    if(sf->frame + sf->num >= sf->framesPerCycle) {
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
    sf->frame = 0;

    if(sf->dir == 1) {
        sf->index = 0;
    } else {
        sf->index = sf->num - 1;
    }
}

SpriteFrames* a_spriteframes_clone(const SpriteFrames* src)
{
    SpriteFrames* const sf = malloc(sizeof(SpriteFrames));

    sf->sprites = a_list_clone(src->sprites);
    sf->spriteArray = (Sprite**)a_list_array(sf->sprites);
    sf->num = src->num;
    sf->frame = 0;
    sf->framesPerCycle = src->framesPerCycle;
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

    sf->spriteArray = (Sprite**)a_list_array(sf->sprites);
    sf->num = a_list_size(sf->sprites);
    a_spriteframes_reset(sf);

    return s;
}
