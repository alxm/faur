/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#pragma once

#include "a2x_pack_sprite.p.h"

#include "a2x_pack_platform.v.h"

struct ASprite {
    APixel* pixels;
    size_t pixelsSize;
    char* nameId;
    int w, wOriginal, wLog2, h;
    APlatformTexture* texture;
};

extern APixel a_sprite__colorKey;
extern APixel a_sprite__colorLimit;
extern APixel a_sprite__colorEnd;

extern void a_sprite__init(void);

extern void a_sprite__boundsFind(const ASprite* Sheet, int X, int Y, int* Width, int* Height);

#define A_SPRITE__NAME(Sprite) (Sprite->nameId ? Sprite->nameId : "Sprite")
#define a_sprite__pixelsGetPixel(s, x, y) (*((s)->pixels + (y) * (s)->w + (x)))
