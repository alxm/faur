/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

static FSpriteLayersLayer* layer_new(FSprite* Sprite, FColorBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    FSpriteLayersLayer* l = f_pool__alloc(F_POOL__SPRITE_LAYER);

    l->sprite = Sprite;
    l->blend = Blend;
    l->r = Red;
    l->g = Green;
    l->b = Blue;
    l->a = Alpha;

    return l;
}

static void layer_free(FSpriteLayersLayer* Layer)
{
    f_pool_release(Layer);
}

static void layer_freeEx(FSpriteLayersLayer* Layer)
{
    f_sprite_free(Layer->sprite);

    f_pool_release(Layer);
}

FSpriteLayers* f_spritelayers_new(void)
{
    FListIntr* l = f_pool__alloc(F_POOL__LISTINTR);

    f_listintr_init(l, FSpriteLayersLayer, listNode);

    return l;
}

void f_spritelayers_free(FSpriteLayers* Layers, bool FreeSprites)
{
    if(Layers == NULL) {
        return;
    }

    if(FreeSprites) {
        f_listintr_apply(Layers, (FCallFree*)layer_freeEx);
    } else {
        f_listintr_apply(Layers, (FCallFree*)layer_free);
    }

    f_pool_release(Layers);
}

void f_spritelayers_clear(FSpriteLayers* Layers, bool FreeSprites)
{
    F__CHECK(Layers != NULL);

    f_listintr_clearEx(Layers,
                       FreeSprites
                        ? (FCallFree*)layer_freeEx : (FCallFree*)layer_free);
}

void f_spritelayers_add(FSpriteLayers* Layers, FSprite* Sprite, FColorBlend Blend, int Red, int Green, int Blue, int Alpha)
{
    F__CHECK(Layers != NULL);
    F__CHECK(Sprite != NULL);
    F__CHECK(Blend < F_COLOR_BLEND_NUM);
    F__CHECK(Red >= 0 && Red <= 255);
    F__CHECK(Green >= 0 && Green <= 255);
    F__CHECK(Blue >= 0 && Blue <= 255);
    F__CHECK(Alpha >= 0 && Alpha <= F_COLOR_ALPHA_MAX);

    f_listintr_addLast(
        Layers, layer_new(Sprite, Blend, Red, Green, Blue, Alpha));
}

void f_spritelayers_blit(const FSpriteLayers* Layers, unsigned Frame, int X, int Y)
{
    F__CHECK(Layers != NULL);

    f_color_push();

    F_LISTINTR_ITERATE(Layers, const FSpriteLayersLayer*, l) {
        f_color_blendSet(l->blend);
        f_color_colorSetRgba(l->r, l->g, l->b, l->a);

        f_sprite_blit(l->sprite, Frame, X, Y);
    }

    f_color_pop();
}
