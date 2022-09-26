/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_VIDEO_GAMEBUINO_VIDEO_V_H
#define F_INC_PLATFORM_VIDEO_GAMEBUINO_VIDEO_V_H

#include "f_gamebuino_video.p.h"

#include "../../graphics/f_pixels.v.h"

extern void f_platform_api_gamebuino__screenInit(void);

extern FPixels* f_platform_api_gamebuino__screenPixelsGet(void);
extern FVecInt f_platform_api_gamebuino__screenSizeGet(void);

extern bool f_platform_api_gamebuino__screenVsyncGet(void);

#endif // F_INC_PLATFORM_VIDEO_GAMEBUINO_VIDEO_V_H
