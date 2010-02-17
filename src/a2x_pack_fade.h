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

#ifndef A2X_PACK_FADE_H
#define A2X_PACK_FADE_H

#include <stdlib.h>

#include "a2x_app_settings.h"

#include "a2x_pack_draw.h"
#include "a2x_pack_fps.h"
#include "a2x_pack_screen.h"

typedef enum FadeSpeed {
    A_FADE_FAST3, A_FADE_FAST2, A_FADE_FAST,
    A_FADE_NORMAL,
    A_FADE_SLOW, A_FADE_SLOW2, A_FADE_SLOW3
} FadeSpeed;

extern void a_fade_toBlack(const FadeSpeed speed);
extern void a_fade_fromBlack(const FadeSpeed speed);

extern void a_fade_screens(const Pixel* const old, const FadeSpeed speed);

extern void a_fade_squaresOn(const Pixel c);
extern void a_fade_squaresOff(const Pixel c);

#endif // A2X_PACK_FADE_H
