/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_spritelayers.v.h"
#include <faur.v.h>

typedef struct {
    ASprite* sprite;
    AColorBlend blend;
    int r, g, b, a;
} ALayer;

static ALayer* layer_new(ASprite* Sprite, AColorBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    ALayer* l = f_mem_malloc(sizeof(ALayer));

    l->sprite = Sprite;
    l->blend = Blend;
    l->r = Red;
    l->g = Green;
    l->b = Blue;
    l->a = Alpha;

    return l;
}

static void layer_free(ALayer* Layer)
{
    f_mem_free(Layer);
}

static void layer_freeEx(ALayer* Layer)
{
    f_sprite_free(Layer->sprite);

    f_mem_free(Layer);
}

ASpriteLayers* f_spritelayers_new(void)
{
    return f_list_new();
}

void f_spritelayers_free(ASpriteLayers* Layers, bool FreeSprites)
{
    if(FreeSprites) {
        f_list_freeEx(Layers, (AFree*)layer_freeEx);
    } else {
        f_list_freeEx(Layers, (AFree*)layer_free);
    }
}

void f_spritelayers_clear(ASpriteLayers* Layers, bool FreeSprites)
{
    if(FreeSprites) {
        f_list_clearEx(Layers, (AFree*)layer_freeEx);
    } else {
        f_list_clearEx(Layers, (AFree*)layer_free);
    }
}

void f_spritelayers_add(ASpriteLayers* Layers, ASprite* Sprite, AColorBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    f_list_addLast(Layers, layer_new(Sprite, Blend, Red, Green, Blue, Alpha));
}

void f_spritelayers_blit(const ASpriteLayers* Layers, unsigned Frame, int X, int Y)
{
    f_color_push();

    A_LIST_ITERATE(Layers, ALayer*, l) {
        f_color_blendSet(l->blend);
        f_color_baseSetRgba(l->r, l->g, l->b, l->a);

        f_sprite_blit(l->sprite, Frame, X, Y);
    }

    f_color_pop();
}
