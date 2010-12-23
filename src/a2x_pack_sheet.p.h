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

#ifndef A2X_PACK_SHEET_PH
#define A2X_PACK_SHEET_PH

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct Sheet Sheet;

#include "a2x_pack_png.p.h"
#include "a2x_pack_screen.p.h"
#include "a2x_pack_types.p.h"

extern Sheet* a_sheet_fromFile(const char* const path);
extern Sheet* a_sheet_fromData(const uint8_t* const data);
extern Sheet* a_sheet_fromSheet(const Sheet* const sheet, const int x, const int y, const int w, const int h);
extern void a_sheet_free(Sheet* const s);

extern void a_sheet_setTransparent(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_sheet_setLimit(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_sheet_setEnd(Sheet* const s, const uint8_t r, const uint8_t g, const uint8_t b);

extern int a_sheet_w(const Sheet* const s);
extern int a_sheet_h(const Sheet* const s);
extern Pixel* a_sheet_data(const Sheet* const s);
extern Pixel a_sheet_getPixel(const Sheet* const s, const int x, const int y);

#endif // A2X_PACK_SHEET_PH
