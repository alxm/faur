/*
    Copyright 2010, 2016, 2018 Alex Margarit <alex@alxm.org>
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

#include "a2x_pack_spritelayers.v.h"

#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"

typedef struct {
    ASprite* sprite;
    APixelBlend blend;
    int r, g, b, a;
} ALayer;

static ALayer* layer_new(ASprite* Sprite, APixelBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    ALayer* l = a_mem_malloc(sizeof(ALayer));

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
    free(Layer);
}

static void layer_freeEx(ALayer* Layer)
{
    a_sprite_free(Layer->sprite);

    free(Layer);
}

ASpriteLayers* a_spritelayers_new(void)
{
    return a_list_new();
}

void a_spritelayers_free(ASpriteLayers* Layers, bool FreeSprites)
{
    if(Layers == NULL) {
        return;
    }

    if(FreeSprites) {
        a_list_freeEx(Layers, (AFree*)layer_freeEx);
    } else {
        a_list_freeEx(Layers, (AFree*)layer_free);
    }
}

void a_spritelayers_clear(ASpriteLayers* Layers, bool FreeSprites)
{
    if(FreeSprites) {
        a_list_clearEx(Layers, (AFree*)layer_freeEx);
    } else {
        a_list_clearEx(Layers, (AFree*)layer_free);
    }
}

void a_spritelayers_add(ASpriteLayers* Layers, ASprite* Sprite, APixelBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    a_list_addLast(Layers, layer_new(Sprite, Blend, Red, Green, Blue, Alpha));
}

void a_spritelayers_blit(ASpriteLayers* Layers, int X, int Y)
{
    a_pixel_push();

    A_LIST_ITERATE(Layers, ALayer*, l) {
        a_pixel_blendSet(l->blend);
        a_pixel_colorSetRgba(l->r, l->g, l->b, l->a);

        a_sprite_blit(l->sprite, X, Y);
    }

    a_pixel_pop();
}
