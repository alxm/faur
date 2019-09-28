/*
    Copyright 2010, 2016 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_SOUND_SOUND_V_H
#define A_INC_SOUND_SOUND_V_H

#include "sound/a_sound.p.h"

#include "general/a_main.v.h"

extern const APack a_pack__sound;

extern void a_sound__tick(void);
extern void a_sound__draw(void);

#endif // A_INC_SOUND_SOUND_V_H
