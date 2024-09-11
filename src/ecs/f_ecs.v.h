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

#ifndef F_INC_ECS_ECS_V_H
#define F_INC_ECS_ECS_V_H

#include "f_ecs.p.h"

#include "../ecs/f_component.v.h"
#include "../ecs/f_system.v.h"
#include "../general/f_init.v.h"

extern const FPack f_pack__ecs;

#if F_CONFIG_ECS_COM_NUM <= 32
    #define F_ECS__BITS_NEW() ((uint32_t)0)
    #define F_ECS__BITS_FREE(Bits)
    #define F_ECS__BITS_SET(Bits, Index) ((Bits) |= (uint32_t)1 << (Index))
    #define F_ECS__BITS_TEST(Bits, Mask) (((Bits) & (Mask)) == (Mask))
#elif F_CONFIG_ECS_COM_NUM <= 64
    #define F_ECS__BITS_NEW() ((uint64_t)0)
    #define F_ECS__BITS_FREE(Bits)
    #define F_ECS__BITS_SET(Bits, Index) ((Bits) |= (uint64_t)1 << (Index))
    #define F_ECS__BITS_TEST(Bits, Mask) (((Bits) & (Mask)) == (Mask))
#else
    #define F_ECS__BITS_NEW() f_bitfield_new(F_CONFIG_ECS_COM_NUM)
    #define F_ECS__BITS_FREE(Bits) f_bitfield_free(Bits)
    #define F_ECS__BITS_SET(Bits, Index) f_bitfield_set((Bits), (Index))
    #define F_ECS__BITS_TEST(Bits, Mask) f_bitfield_testMask((Bits), (Mask))
#endif

#endif // F_INC_ECS_ECS_V_H
