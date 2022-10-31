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

extern FCallApi_InputPoll f_platform_api_sdl__inputPoll;

extern FCallApi_InputKeyGet f_platform_api_sdl__inputKeyGet;

extern FCallApi_InputButtonGet f_platform_api_sdl__inputButtonGet;
extern FCallApi_InputButtonPressGet f_platform_api_sdl__inputButtonPressGet;

extern FCallApi_InputAnalogGet f_platform_api_sdl__inputAnalogGet;
extern FCallApi_InputAnalogValueGet f_platform_api_sdl__inputAnalogValueGet;

extern FCallApi_InputTouchCoordsGet f_platform_api_sdl__inputTouchCoordsGet;
extern FCallApi_InputTouchDeltaGet f_platform_api_sdl__inputTouchDeltaGet;
extern FCallApi_InputTouchTapGet f_platform_api_sdl__inputTouchTapGet;

extern FCallApi_InputControllerClaim f_platform_api_sdl__inputControllerClaim;
extern FCallApi_InputControllerRelease f_platform_api_sdl__inputControllerRelease;

#endif // F_INC_PLATFORM_INPUT_SDL_INPUT_V_H
