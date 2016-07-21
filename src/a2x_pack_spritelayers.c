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
    APixelBlend_t blend;
    uint8_t r, g, b, a;
} ALayer;

ASpriteLayers* a_spritelayers_new(void)
{
    return a_list_new();
}

void a_spritelayers_free(ASpriteLayers* s)
{
    A_LIST_ITERATE(s, ALayer, l) {
        free(l);
    }

    a_list_free(s);
}

void a_spritelayers_add(ASpriteLayers* s, const ASprite* sprite, APixelBlend_t blend, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    ALayer* const l = a_mem_malloc(sizeof(ALayer));

    l->sprite = sprite;
    l->blend = blend;
    l->r = r;
    l->g = g;
    l->b = b;
    l->a = a;

    a_list_addLast(s, l);
}

void a_spritelayers_blit(ASpriteLayers* s, int x, int y)
{
    A_LIST_ITERATE(s, ALayer, l) {
        a_pixel_setBlend(l->blend);
        a_pixel_setRGBA(l->r, l->g, l->b, l->a);

        a_blit(l->sprite, x, y);
    }
}
