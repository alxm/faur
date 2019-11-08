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

#include "f_music.v.h"
#include <faur.v.h>

#if F_CONFIG_FEATURE_MUSIC
FMusic* f_music_new(const char* Path)
{
    FPlatformMusic* m = f_platform_api__soundMusicNew(Path);

    if(m == NULL) {
        f_out__error("f_music_new(%s): Cannot open file", Path);
    }

    return m;
}

void f_music_free(FMusic* Music)
{
    if(Music) {
        f_platform_api__soundMusicFree(Music);
    }
}

void f_music_play(FMusic* Music)
{
    if(f_platform_api__soundMuteGet()) {
        return;
    }

    f_platform_api__soundMusicPlay(Music);
}

void f_music_stop(void)
{
    f_platform_api__soundMusicStop();
}
#else // !F_CONFIG_FEATURE_MUSIC
FMusic* f_music_new(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

void f_music_free(FMusic* Music)
{
    F_UNUSED(Music);
}

void f_music_play(FMusic* Music)
{
    F_UNUSED(Music);
}

void f_music_stop(void)
{
}
#endif // !F_CONFIG_FEATURE_MUSIC
