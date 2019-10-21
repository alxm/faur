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

#include "f_sample.v.h"
#include <faur.v.h>

#if F_CONFIG_SOUND_ENABLED
FSample* f_sample_new(const char* Path)
{
    FPlatformSample* s = NULL;

    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        s = f_platform_api__soundSampleNewFromFile(Path);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        const FEmbeddedFile* e = f_embed__fileGet(Path);

        s = f_platform_api__soundSampleNewFromData(e->buffer, (int)e->size);
    } else {
        F__FATAL("f_sample_new(%s): File does not exist", Path);
    }

    if(s == NULL) {
        F__FATAL("f_sample_new(%s): Cannot open file", Path);
    }

    return s;
}

void f_sample_free(FSample* Sample)
{
    if(Sample) {
        f_platform_api__soundSampleFree(Sample);
    }
}
#else // !F_CONFIG_SOUND_ENABLED
FSample* f_sample_new(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

void f_sample_free(FSample* Sample)
{
    F_UNUSED(Sample);
}
#endif // !F_CONFIG_SOUND_ENABLED
