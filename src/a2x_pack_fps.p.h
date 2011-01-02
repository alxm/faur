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

#ifndef A2X_PACK_FPS_PH
#define A2X_PACK_FPS_PH

#include "a2x_app_includes.h"

extern void a_fps_start(void);
extern void a_fps_end(void);

extern uint32_t a_fps_getFps(void);
extern uint32_t a_fps_getMaxFps(void);
extern uint32_t a_fps_getCounter(void);

#endif // A2X_PACK_FPS_PH
