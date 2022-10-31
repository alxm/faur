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

#ifndef F_INC_PLATFORM_VIDEO_ODROID_GO_VIDEO_V_H
#define F_INC_PLATFORM_VIDEO_ODROID_GO_VIDEO_V_H

#include "f_odroid_go_video.p.h"

#include "../f_platform.v.h"
#include "../../graphics/f_pixels.v.h"

extern FCallApi_ScreenInit f_platform_api_odroidgo__screenInit;

extern FCallApi_ScreenClear f_platform_api_odroidgo__screenClear;
extern FCallApi_ScreenShow f_platform_api_odroidgo__screenShow;

extern FCallApi_ScreenPixelsGet f_platform_api_odroidgo__screenPixelsGet;
extern FCallApi_ScreenSizeGet f_platform_api_odroidgo__screenSizeGet;

#endif // F_INC_PLATFORM_VIDEO_ODROID_GO_VIDEO_V_H
