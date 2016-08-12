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

#include "a2x_pack_spritelayers.v.h"

typedef struct ALayer {
    const ASprite* sprite;
    APixelBlend blend;
    uint8_t r, g, b;
    unsigned int a;
} ALayer;

ASpriteLayers* a_spritelayers_new(void)
{
    return a_list_new();
}

void a_spritelayers_free(ASpriteLayers* Layers)
{
    ALayer* l;

    A_LIST_ITERATE(Layers, l) {
        free(l);
    }

    a_list_free(Layers);
}

void a_spritelayers_add(ASpriteLayers* Layers, const ASprite* Sprite, APixelBlend Blend, uint8_t Red, uint8_t Green, uint8_t Blue, unsigned int Alpha)
{
    ALayer* const l = a_mem_malloc(sizeof(ALayer));

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
    ALayer* l;

    A_LIST_ITERATE(Layers, l) {
        a_pixel_setBlend(l->blend);
        a_pixel_setRGBA(l->r, l->g, l->b, l->a);

        a_blit(l->sprite, X, Y);
    }
}
