/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#include "f_channel.v.h"
#include <faur.v.h>

int f_channel_new(void)
{
    return f_platform_api__soundSampleChannelGet();
}

void f_channel_playStart(int Channel, const FSample* Sample, unsigned Flags)
{
    if(f_platform_api__soundMuteGet()) {
        return;
    }

    if(F_FLAGS_TEST_ANY(Flags, F_CHANNEL_PLAY_RESTART)) {
        f_platform_api__soundSampleStop(Channel);
    } else if(F_FLAGS_TEST_ANY(Flags, F_CHANNEL_PLAY_YIELD)
        && f_platform_api__soundSampleIsPlaying(Channel)) {

        return;
    }

    #if !F_CONFIG_SOUND_SAMPLE_PRE_INIT
        if(Sample->size != 0 && *(Sample->u.platformIndirect) == NULL) {
            *(Sample->u.platformIndirect) =
                f_platform_api__soundSampleNewFromData(
                    Sample->buffer, Sample->size);
        }
    #endif

    f_platform_api__soundSamplePlay(
        Sample,
        Channel,
        F_FLAGS_TEST_ANY(Flags, F_CHANNEL_PLAY_LOOP));
}

void f_channel_playStop(int Channel)
{
    f_platform_api__soundSampleStop(Channel);
}

bool f_channel_playGet(int Channel)
{
    return f_platform_api__soundSampleIsPlaying(Channel);
}
