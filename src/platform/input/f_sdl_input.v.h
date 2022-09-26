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

#ifndef F_INC_PLATFORM_INPUT_SDL_INPUT_V_H
#define F_INC_PLATFORM_INPUT_SDL_INPUT_V_H

#include "f_sdl_input.p.h"

#include "../f_platform.v.h"

extern void f_platform_sdl_input__init(void);
extern void f_platform_sdl_input__uninit(void);

extern void f_platform_api_sdl__inputPoll(void);

extern const FPlatformButton* f_platform_api_sdl__inputKeyGet(FKeyId Id);

extern const FPlatformButton* f_platform_api_sdl__inputButtonGet(const FPlatformController* Controller, FButtonId Id);
extern bool f_platform_api_sdl__inputButtonPressGet(const FPlatformButton* Button);

extern const FPlatformAnalog* f_platform_api_sdl__inputAnalogGet(const FPlatformController* Controller, FAnalogId Id);
extern int f_platform_api_sdl__inputAnalogValueGet(const FPlatformAnalog* Analog);

extern FVecInt f_platform_api_sdl__inputTouchCoordsGet(void);
extern FVecInt f_platform_api_sdl__inputTouchDeltaGet(void);
extern bool f_platform_api_sdl__inputTouchTapGet(void);

extern FPlatformController* f_platform_api_sdl__inputControllerClaim(FCallControllerBind* Callback);
extern void f_platform_api_sdl__inputControllerRelease(FPlatformController* Controller);

#endif // F_INC_PLATFORM_INPUT_SDL_INPUT_V_H
