/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include <a2x.v.h>
#include <time.h>

static ARandomPrng* g_rand;
static ARandomPrngSeed* g_srand;
static unsigned g_seed;

static void a_random__init(void)
{
    a_random_generatorReset();
}

const APack a_pack__random = {
    "Random",
    {
        [0] = a_random__init,
    },
    {
        NULL,
    },
};

void a_random_generatorSet(ARandomPrng* Rand, ARandomPrngSeed* Srand)
{
    g_rand = Rand;
    g_srand = Srand;

    time_t t = time(NULL);

    if(t < 0) {
        a_random_seedSet(0);
    } else {
        a_random_seedSet((unsigned)t);
    }
}

void a_random_generatorReset(void)
{
    a_random_generatorSet(rand, srand);
}

unsigned a_random_seedGet(void)
{
    return g_seed;
}

void a_random_seedSet(unsigned Seed)
{
    g_seed = Seed;

    if(g_srand != NULL) {
        g_srand(Seed);
    }

    a_out_info("a_random_seedSet(%u)", Seed);
}

int a_random_int(int Max)
{
    if(Max <= 0) {
        A__FATAL("a_random_int(%d): Invalid arg", Max);
    }

    return g_rand() % Max;
}

unsigned a_random_intu(unsigned Max)
{
    if(Max == 0) {
        A__FATAL("a_random_intu(0): Invalid arg");
    }

    return (unsigned)g_rand() % Max;
}

int a_random_range(int Min, int Max)
{
    if(Min >= Max) {
        A__FATAL("a_random_range(%d, %d): Invalid args", Min, Max);
    }

    return Min + (g_rand() % (Max - Min));
}

unsigned a_random_rangeu(unsigned Min, unsigned Max)
{
    return Min + ((unsigned)g_rand() % (Max - Min));
}

bool a_random_chance(int Something, int OutOf)
{
    if(Something > OutOf) {
        A__FATAL("a_random_chance(%d, %d): Invalid args", Something, OutOf);
    }

    return a_random_int(OutOf) < Something;
}

bool a_random_chanceu(unsigned Something, unsigned OutOf)
{
    if(Something > OutOf) {
        A__FATAL("a_random_chanceu(%d, %d): Invalid args", Something, OutOf);
    }

    return a_random_intu(OutOf) < Something;
}
