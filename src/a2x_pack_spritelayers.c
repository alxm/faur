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

#include "a2x_pack_spritelayers.v.h"

typedef struct ALayer {
    ASprite* sprite;
    APixelBlend blend;
    int r, g, b, a;
} ALayer;

ASpriteLayers* a_spritelayers_new(void)
{
    return a_list_new();
}

void a_spritelayers_free(ASpriteLayers* Layers, bool FreeSprites)
{
    A_LIST_ITERATE(Layers, ALayer*, l) {
        if(FreeSprites) {
            a_sprite_free(l->sprite);
        }

        free(l);
    }

    a_list_free(Layers);
}

void a_spritelayers_add(ASpriteLayers* Layers, ASprite* Sprite, APixelBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    ALayer* l = a_mem_malloc(sizeof(ALayer));

    l->sprite = Sprite;
    l->blend = Blend;
    l->r = Red;
    l->g = Green;
    l->b = Blue;
    l->a = Alpha;

    a_list_addLast(Layers, l);
}

void a_spritelayers_blit(ASpriteLayers* Layers, int X, int Y)
{
    a_pixel_push();

    A_LIST_ITERATE(Layers, ALayer*, l) {
        a_pixel_setBlend(l->blend);
        a_pixel_setRGBA(l->r, l->g, l->b, l->a);

        a_sprite_blit(l->sprite, X, Y);
    }

    a_pixel_pop();
}
