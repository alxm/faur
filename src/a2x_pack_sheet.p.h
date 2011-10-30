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

#include "a2x_app_includes.h"

typedef struct Sheet Sheet;

#include "a2x_pack_pixel.p.h"

extern Sheet* a_sheet_fromFile(const char* path);
extern Sheet* a_sheet_fromData(const uint8_t* data);
extern Sheet* a_sheet_fromSheet(const Sheet* sheet, int x, int y, int w, int h);
extern void a_sheet_free(Sheet* s);

extern void a_sheet_setTransparent(Sheet* s, uint8_t r, uint8_t g, uint8_t b);
extern void a_sheet_setLimit(Sheet* s, uint8_t r, uint8_t g, uint8_t b);
extern void a_sheet_setEnd(Sheet* s, uint8_t r, uint8_t g, uint8_t b);

extern int a_sheet_w(const Sheet* s);
extern int a_sheet_h(const Sheet* s);
extern Pixel* a_sheet_data(const Sheet* s);
extern Pixel a_sheet_getPixel(const Sheet* s, int x, int y);

#endif // A2X_PACK_SHEET_PH
