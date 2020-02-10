/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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
    FSample* s = f_mem_zalloc(sizeof(FSample));

    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        s->platform = f_platform_api__soundSampleNewFromFile(Path);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        const FEmbeddedFile* e = f_embed__fileGet(Path);

        s->platform =
            f_platform_api__soundSampleNewFromData(e->buffer, e->size);
    } else {
        F__FATAL("f_sample_new(%s): File does not exist", Path);
    }

    if(s->platform == NULL) {
        F__FATAL("f_sample_new(%s): Cannot open file", Path);
    }

    return s;
}

void f_sample_free(FSample* Sample)
{
    if(Sample == NULL) {
        return;
    }

    f_platform_api__soundSampleFree(Sample->platform);

    if(Sample->size == 0) {
        f_mem_free(Sample);
    } else {
        Sample->platform = NULL;
    }
}

void f_sample__lazyInit(FSample* Sample)
{
    if(Sample->size > 0 && Sample->platform == NULL) {
        Sample->platform = f_platform_api__soundSampleNewFromData(
                            Sample->buffer, Sample->size);
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
