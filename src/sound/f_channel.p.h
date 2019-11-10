/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_SOUND_CHANNEL_P_H
#define F_INC_SOUND_CHANNEL_P_H

#include "../general/f_system_includes.h"

typedef enum {
    F_CHANNEL_PLAY_NORMAL = 0,
    F_CHANNEL_PLAY_LOOP = F_FLAGS_BIT(0),
    F_CHANNEL_PLAY_RESTART = F_FLAGS_BIT(1),
    F_CHANNEL_PLAY_YIELD = F_FLAGS_BIT(2),
} FChannelFlags;

#include "../sound/f_sample.p.h"

#define F_CHANNEL_ANY -1

extern int f_channel_new(void);
extern void f_channel_play(int Channel, const FSample* Sample, FChannelFlags Flags);
extern void f_channel_stop(int Channel);
extern bool f_channel_isPlaying(int Channel);

#endif // F_INC_SOUND_CHANNEL_P_H
