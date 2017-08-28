/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_sdl_video.p.h"

#if A_CONFIG_RENDER_SDL2
    typedef struct ASdlTexture ASdlTexture;
#endif

#include "a2x_pack_pixel.v.h"

extern void a_sdl_video__init(void);
extern void a_sdl_video__uninit(void);

extern void a_sdl_screen__set(int Width, int Height, bool FullScreen);
extern void a_sdl_screen__show(void);
extern void a_sdl_screen__setFullScreen(bool FullScreen);

#if A_CONFIG_RENDER_SDL2
extern void a_sdl_video__getFullResolution(int* Width, int* Height);

extern void a_sdl_render__setDrawColor(void);
extern void a_sdl_render__setBlendMode(void);
extern void a_sdl_render__clear(void);

extern void a_sdl_render__drawPoint(int X, int Y);
extern void a_sdl_render__drawLine(int X1, int Y1, int X2, int Y2);
extern void a_sdl_render__drawRectangle(int X, int Y, int Width, int Height);
extern void a_sdl_render__drawCircle(int X, int Y, int Radius);

extern ASdlTexture* a_sdl_render__textureMakeScreen(int Width, int Height);
extern ASdlTexture* a_sdl_render__textureMakeSprite(const APixel* Pixels, int Width, int Height);
extern void a_sdl_render__textureFree(ASdlTexture* Texture);

extern void a_sdl_render__textureBlit(ASdlTexture* Texture, int X, int Y, bool FillFlat);
extern void a_sdl_render__textureBlitEx(ASdlTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat);

extern void a_sdl_render__targetSet(ASdlTexture* Texture);
extern void a_sdl_render__targetGetPixels(APixel* Pixels, int Width);
extern void a_sdl_render__targetSetClip(int X, int Y, int Width, int Height);
#endif
