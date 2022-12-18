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

#include "f_random.v.h"
#include <faur.v.h>
#include <time.h>

static FCallRandomPrng* g_rand;
static FCallRandomPrngSeed* g_srand;
static unsigned g_seed;

static void f_random__init(void)
{
    f_random_generatorReset();
}

const FPack f_pack__random = {
    "Random",
    f_random__init,
    NULL,
};

void f_random_generatorSet(FCallRandomPrng* Rand, FCallRandomPrngSeed* Srand)
{
    F__CHECK(Rand != NULL);

    g_rand = Rand;
    g_srand = Srand;

    #if F_CONFIG_TRAIT_NO_SEEDING
        f_random_seedSet(0);
    #else
        f_random_seedSet((unsigned)time(NULL));
    #endif
}

void f_random_generatorReset(void)
{
    f_random_generatorSet(rand, srand);
}

unsigned f_random_seedGet(void)
{
    return g_seed;
}

void f_random_seedSet(unsigned Seed)
{
    g_seed = Seed;

    if(g_srand != NULL) {
        g_srand(Seed);
    }

    f_out__info("f_random_seedSet(%u)", Seed);
}

int f_random_int(int Max)
{
    F__CHECK(Max > 0);

    return g_rand() % Max;
}

unsigned f_random_intu(unsigned Max)
{
    F__CHECK(Max > 0);

    return (unsigned)g_rand() % Max;
}

int f_random_range(int Min, int Max)
{
    F__CHECK((Max - Min) > 0);

    return Min + (g_rand() % (Max - Min));
}

unsigned f_random_rangeu(unsigned Min, unsigned Max)
{
    F__CHECK((Max - Min) > 0);

    return Min + ((unsigned)g_rand() % (Max - Min));
}

bool f_random_chance(int Something, int OutOf)
{
    F__CHECK(Something >= 0);
    F__CHECK(OutOf > 0);
    F__CHECK(Something <= OutOf);

    return f_random_int(OutOf) < Something;
}

bool f_random_chanceu(unsigned Something, unsigned OutOf)
{
    F__CHECK(OutOf > 0);
    F__CHECK(Something <= OutOf);

    return f_random_intu(OutOf) < Something;
}
