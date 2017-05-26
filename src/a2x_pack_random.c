/*
    Copyright 2016, 2017 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_random.v.h"

static ARandomPrng* g_rand;
static ARandomPrngSeed* g_srand;
static unsigned g_seed;

void a_random__init(void)
{
    time_t t = time(NULL);

    a_random_resetGenerator();

    if(t < 0) {
        a_random_setSeed(0);
    } else {
        a_random_setSeed((unsigned)t);
    }
}

void a_random_setGenerator(ARandomPrng* Rand, ARandomPrngSeed* Srand)
{
    g_rand = Rand;
    g_srand = Srand;
}

void a_random_resetGenerator(void)
{
    a_random_setGenerator(rand, srand);
}

unsigned a_random_getSeed(void)
{
    return g_seed;
}

void a_random_setSeed(unsigned Seed)
{
    g_seed = Seed;

    if(g_srand != NULL) {
        g_srand(Seed);
    }
}

int a_random_getInt(int Max)
{
    if(Max == 0) {
        a_out__fatal("a_random_getInt: invalid arg 0");
    }

    return g_rand() % Max;
}

int a_random_getInRange(int Min, int Max)
{
    if(Min >= Max) {
        a_out__fatal("a_random_getInRange: invalid args %d, %d", Min, Max);
    }

    return Min + (g_rand() % (Max - Min));
}
