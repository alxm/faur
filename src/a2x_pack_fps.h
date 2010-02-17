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

#ifndef A2X_PACK_FPS_H
#define A2X_PACK_FPS_H

#include "a2x_app_settings.h"

#include "a2x_pack_input.h"
#include "a2x_pack_math.h"
#include "a2x_pack_screen.h"
#include "a2x_pack_sound.h"
#include "a2x_pack_time.h"

extern void a__fps_set(void);

extern void a_fps_start(void);
extern void a_fps_end(void);

extern uint32_t a_fps_fps(void);
extern uint32_t a_fps_max(void);
extern uint32_t a_fps_counter(void);

#endif // A2X_PACK_FPS_H
