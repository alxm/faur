/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "platform/a_no_sound.v.h"

#if !A_CONFIG_SOUND_ENABLED
#include "platform/a_platform.v.h"

bool a_platform_api__soundMuteGet(void)
{
    return true;
}

void a_platform_api__soundMuteFlip(void)
{
    return;
}

int a_platform_api__soundVolumeGetMax(void)
{
    return 0;
}

APlatformSoundMusic* a_platform_api__soundMusicNew(const char* Path)
{
    A_UNUSED(Path);

    return NULL;
}

void a_platform_api__soundMusicFree(APlatformSoundMusic* Music)
{
    A_UNUSED(Music);
}

void a_platform_api__soundMusicVolumeSet(int Volume)
{
    A_UNUSED(Volume);
}

void a_platform_api__soundMusicPlay(APlatformSoundMusic* Music)
{
    A_UNUSED(Music);
}

void a_platform_api__soundMusicStop(void)
{
    return;
}

APlatformSoundSample* a_platform_api__soundSampleNewFromFile(const char* Path)
{
    A_UNUSED(Path);

    return NULL;
}

APlatformSoundSample* a_platform_api__soundSampleNewFromData(const uint8_t* Data, int Size)
{
    A_UNUSED(Data);
    A_UNUSED(Size);

    return NULL;
}

void a_platform_api__soundSampleFree(APlatformSoundSample* Sample)
{
    A_UNUSED(Sample);
}

void a_platform_api__soundSampleVolumeSet(APlatformSoundSample* Sample, int Volume)
{
    A_UNUSED(Sample);
    A_UNUSED(Volume);
}

void a_platform_api__soundSampleVolumeSetAll(int Volume)
{
    A_UNUSED(Volume);
}

void a_platform_api__soundSamplePlay(APlatformSoundSample* Sample, int Channel, bool Loop)
{
    A_UNUSED(Sample);
    A_UNUSED(Channel);
    A_UNUSED(Loop);
}

void a_platform_api__soundSampleStop(int Channel)
{
    A_UNUSED(Channel);
}

bool a_platform_api__soundSampleIsPlaying(int Channel)
{
    A_UNUSED(Channel);

    return false;
}

int a_platform_api__soundSampleChannelGet(void)
{
    return -1;
}
#endif // !A_CONFIG_SOUND_ENABLED
