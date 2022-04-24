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

#ifndef F_INC_MATH_RANDOM_P_H
#define F_INC_MATH_RANDOM_P_H

#include "../general/f_system_includes.h"

typedef int FCallRandomPrng(void);
typedef void FCallRandomPrngSeed(unsigned Seed);

extern void f_random_generatorSet(FCallRandomPrng* Rand, FCallRandomPrngSeed* Srand);
extern void f_random_generatorReset(void);

extern unsigned f_random_seedGet(void);
extern void f_random_seedSet(unsigned Seed);

extern int f_random_int(int Max);
extern unsigned f_random_intu(unsigned Max);

extern int f_random_range(int Min, int Max);
extern unsigned f_random_rangeu(unsigned Min, unsigned Max);

extern bool f_random_chance(int Something, int OutOf);
extern bool f_random_chanceu(unsigned Something, unsigned OutOf);

#endif // F_INC_MATH_RANDOM_P_H
