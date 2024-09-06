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

#ifndef F_INC_SOUND_SAMPLE_V_H
#define F_INC_SOUND_SAMPLE_V_H

#include "../sound/f_sample.p.h"

#include "../platform/f_platform.v.h"

struct FSample {
    unsigned size;
    const uint8_t* buffer;
    #if F_CONFIG_SOUND_SAMPLE_HAS_RUNTIME_OBJECT
        union {
            FPlatformSample* platform;
            FPlatformSample** platformIndirect;
        } u;
    #endif
};

extern void f_sample__lazyInit(FSample* Sample);

#endif // F_INC_SOUND_SAMPLE_V_H
