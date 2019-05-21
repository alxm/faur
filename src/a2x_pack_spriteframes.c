/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_spriteframes.v.h"

#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_random.v.h"
#include "a2x_pack_sprite.v.h"
#include "a2x_pack_str.v.h"

struct ASpriteFrames {
    ATimer* timer;
    AList* sprites;
    ASprite** spriteArray;
    unsigned num;
    unsigned index;
    bool forward;
};

ASpriteFrames* a_spriteframes_newBlank(void)
{
    ASpriteFrames* f = a_mem_zalloc(sizeof(ASpriteFrames));

    f->sprites = a_list_new();
    f->forward = true;

    return f;
}

ASpriteFrames* a_spriteframes_newFromPng(const char* Path, int CellWidth, int CellHeight)
{
    ASprite* s = a_sprite_newFromPng(Path);

    if(CellWidth < 1 || CellHeight < 1) {
        char* suffix = a_str_suffixGetFromLast(Path, '_');

        if(suffix) {
            if(sscanf(suffix, "grid%dx%d.png", &CellWidth, &CellHeight) != 2) {
                CellWidth = 0;
                CellHeight = 0;
            }

            free(suffix);
        }
    }

    ASpriteFrames* f = a_spriteframes_newFromSprite(
                        s, 0, 0, CellWidth, CellHeight);

    a_sprite_free(s);

    return f;
}

ASpriteFrames* a_spriteframes_newFromSprite(const ASprite* Sheet, int X, int Y, int CellWidth, int CellHeight)
{
    AVectorInt gridDim = a_pixels__boundsFind(Sheet->pixels, X, Y);

    return a_spriteframes_newFromSpriteEx(
            Sheet, X, Y, CellWidth, CellHeight, gridDim.x, gridDim.y);
}

ASpriteFrames* a_spriteframes_newFromSpriteEx(const ASprite* Sheet, int X, int Y, int CellWidth, int CellHeight, int GridWidth, int GridHeight)
{
    ASpriteFrames* f = a_spriteframes_newBlank();

    if(CellWidth < 1 || CellHeight < 1) {
        CellWidth = GridWidth;
        CellHeight = GridHeight;
    }

    int endX = X + GridWidth - (GridWidth % CellWidth);
    int endY = Y + GridHeight - (GridHeight % CellHeight);

    for(int y = Y; y < endY; y += CellHeight) {
        for(int x = X; x < endX; x += CellWidth) {
            ASprite* s = a_sprite_newFromSpriteEx(
                            Sheet, x, y, CellWidth, CellHeight);

            a_list_addLast(f->sprites, s);
        }
    }

    f->spriteArray = (ASprite**)a_list_toArray(f->sprites);
    f->num = a_list_sizeGet(f->sprites);

    return f;
}

ASpriteFrames* a_spriteframes_dup(const ASpriteFrames* Frames, bool DupSprites)
{
    ASpriteFrames* f = a_mem_malloc(sizeof(ASpriteFrames));

    if(Frames->timer) {
        f->timer = a_timer_dup(Frames->timer);
    } else {
        f->timer = NULL;
    }

    if(DupSprites) {
        f->sprites = a_list_new();

        A_LIST_ITERATE(Frames->sprites, ASprite*, s) {
            a_list_addLast(f->sprites, a_sprite_dup(s));
        }
    } else {
        f->sprites = a_list_dup(Frames->sprites);
    }

    f->spriteArray = (ASprite**)a_list_toArray(f->sprites);
    f->num = Frames->num;
    f->index = 0;
    f->forward = true;

    a_spriteframes_start(f);

    return f;
}

void a_spriteframes_free(ASpriteFrames* Frames, bool FreeSprites)
{
    if(Frames == NULL) {
        return;
    }

    a_timer_free(Frames->timer);

    if(FreeSprites) {
        a_list_freeEx(Frames->sprites, (AFree*)a_sprite_free);
    } else {
        a_list_free(Frames->sprites);
    }

    free(Frames->spriteArray);
    free(Frames);
}

void a_spriteframes_clear(ASpriteFrames* Frames, bool FreeSprites)
{
    if(FreeSprites) {
        a_list_clearEx(Frames->sprites, (AFree*)a_sprite_free);
    } else {
        a_list_clear(Frames->sprites);
    }

    a_spriteframes_reset(Frames);
}

void a_spriteframes_reset(ASpriteFrames* Frames)
{
    if(Frames->forward) {
        Frames->index = 0;
    } else {
        Frames->index = Frames->num - 1;
    }

    a_spriteframes_start(Frames);
}

void a_spriteframes_randomize(ASpriteFrames* Frames)
{
    a_spriteframes_indexSet(Frames, a_random_intu(Frames->num));
}

static void addedSprite(ASpriteFrames* Frames)
{
    Frames->num++;

    free(Frames->spriteArray);
    Frames->spriteArray = (ASprite**)a_list_toArray(Frames->sprites);

    a_spriteframes_reset(Frames);
}

void a_spriteframes_addFirst(ASpriteFrames* Frames, ASprite* Sprite)
{
    a_list_addFirst(Frames->sprites, Sprite);

    addedSprite(Frames);
}

void a_spriteframes_addLast(ASpriteFrames* Frames, ASprite* Sprite)
{
    a_list_addLast(Frames->sprites, Sprite);

    addedSprite(Frames);
}

static void removedSprite(ASpriteFrames* Frames)
{
    Frames->num--;

    free(Frames->spriteArray);
    Frames->spriteArray = (ASprite**)a_list_toArray(Frames->sprites);

    a_spriteframes_reset(Frames);
}

ASprite* a_spriteframes_removeFirst(ASpriteFrames* Frames)
{
    ASprite* s = a_list_removeFirst(Frames->sprites);

    removedSprite(Frames);

    return s;
}

ASprite* a_spriteframes_removeLast(ASpriteFrames* Frames)
{
    ASprite* s = a_list_removeLast(Frames->sprites);

    removedSprite(Frames);

    return s;
}

ASprite* a_spriteframes_getNext(ASpriteFrames* Frames)
{
    const unsigned oldindex = Frames->index;
    bool advance = false;

    if(Frames->timer) {
        if(a_timer_expiredGet(Frames->timer)) {
            a_timer_expiredClear(Frames->timer);
            advance = true;
        }
    } else {
        advance = true;
    }

    if(advance) {
        if(Frames->forward && ++Frames->index == Frames->num) {
            Frames->index = 0;
        } else if(!Frames->forward && Frames->index-- == 0) {
            Frames->index = Frames->num - 1;
        }
    }

    return Frames->spriteArray[oldindex];
}

ASprite* a_spriteframes_getCurrent(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[Frames->index];
}

ASprite* a_spriteframes_getByIndex(const ASpriteFrames* Frames, unsigned Index)
{
    return Frames->spriteArray[Index];
}

ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames)
{
    return Frames->spriteArray[a_random_intu(Frames->num)];
}

AList* a_spriteframes_framesListGet(const ASpriteFrames* Frames)
{
    return Frames->sprites;
}

unsigned a_spriteframes_framesNumGet(const ASpriteFrames* Frames)
{
    return Frames->num;
}

unsigned a_spriteframes_indexGet(const ASpriteFrames* Frames)
{
    return Frames->index;
}

void a_spriteframes_indexSet(ASpriteFrames* Frames, unsigned Index)
{
    Frames->index = Index;

    a_spriteframes_start(Frames);
}

void a_spriteframes_directionSet(ASpriteFrames* Frames, bool Forward)
{
    Frames->forward = Forward;
}

void a_spriteframes_directionFlip(ASpriteFrames* Frames)
{
    Frames->forward = !Frames->forward;
}

unsigned a_spriteframes_speedGet(const ASpriteFrames* Frames)
{
    return Frames->timer ? a_timer_periodGet(Frames->timer) : 0;
}

unsigned a_spriteframes_speedGetUnitsPerCycle(const ASpriteFrames* Frames)
{
    unsigned frameUnits = a_spriteframes_speedGet(Frames);

    if(frameUnits == 0) {
        return Frames->num;
    }

    return Frames->num * frameUnits;
}

void a_spriteframes_speedSet(ASpriteFrames* Frames, ATimerType Units, unsigned TimePerFrame)
{
    if(Frames->timer) {
        a_timer_periodSet(Frames->timer, TimePerFrame);
    } else {
        Frames->timer = a_timer_new(Units, TimePerFrame, true);
    }

    a_spriteframes_start(Frames);
}

void a_spriteframes_start(ASpriteFrames* Frames)
{
    if(Frames->timer) {
        a_timer_start(Frames->timer);
    }
}

void a_spriteframes_pause(ASpriteFrames* Frames)
{
    if(Frames->timer) {
        a_timer_stop(Frames->timer);
    }
}
