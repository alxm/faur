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

#include "a2x_pack_layersprite.p.h"
#include "a2x_pack_layersprite.v.h"

typedef struct Layer Layer;

struct LayerSprite {
    List* layers;
};

struct Layer {
    Sprite* sprite;
    PixelBlend_t blend;
    uint8_t r, g, b, a;
};

LayerSprite* a_layersprite_make(void)
{
    LayerSprite* const ls = malloc(sizeof(LayerSprite));

    ls->layers = a_list_set();

    return ls;
}

void a_layersprite_free(LayerSprite* const ls)
{
    a_list_freeContent(ls->layers);

    free(ls);
}

void a_layersprite_add(LayerSprite* const ls, Sprite* const sprite, const PixelBlend_t blend, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    Layer* const l = malloc(sizeof(Layer));

    l->sprite = sprite;
    l->blend = blend;
    l->r = r;
    l->g = g;
    l->b = b;
    l->a = a;

    a_list_addLast(ls->layers, l);
}

void a_layersprite_blit(LayerSprite* const ls, const int x, const int y)
{
    List* const layers = ls->layers;

    while(a_list_iterate(layers)) {
        Layer* const l = a_list_current(layers);

        a_pixel_setBlend(l->blend);
        a_pixel_setRGB(l->r, l->g, l->b);
        a_pixel_setAlpha(l->a);

        a_blit(l->sprite, x, y);
    }
}
