/*
    Copyright 2010, 2016, 2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_SOUND_SOUND_P_H
#define A_INC_SOUND_SOUND_P_H

#include "general/f_system_includes.h"

typedef void AMusic;
typedef void ASample;

typedef enum {
    A_CHANNEL_NORMAL = 0,
    A_CHANNEL_LOOP = A_FLAGS_BIT(0),
    A_CHANNEL_RESTART = A_FLAGS_BIT(1),
    A_CHANNEL_YIELD = A_FLAGS_BIT(2),
} AChannelFlags;

#define A_CHANNEL_ANY -1

extern AMusic* f_music_new(const char* Path);
extern void f_music_free(AMusic* Music);

extern void f_music_play(AMusic* Music);
extern void f_music_stop(void);

extern ASample* f_sample_new(const char* Path);
extern void f_sample_free(ASample* Sample);

extern int f_channel_new(void);
extern void f_channel_play(int Channel, ASample* Sample, AChannelFlags Flags);
extern void f_channel_stop(int Channel);
extern bool f_channel_isPlaying(int Channel);

#endif // A_INC_SOUND_SOUND_P_H
