/*
    Copyright 2010, 2016-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_STRINGS_STR_P_H
#define A_INC_STRINGS_STR_P_H

#include "general/a_system_includes.h"

#include "data/a_list.p.h"

extern const char* a_str_fmt(char* Buffer, size_t Size, bool OverflowOk, const char* Format, ...);
extern const char* a_str_fmtv(char* Buffer, size_t Size, bool OverflowOk, const char* Format, va_list Args);
extern const char* a_str_fmt512(const char* Format, ...);

extern char* a_str_merge(const char* String1, ...);
extern char* a_str_dup(const char* String);
extern char* a_str_trim(const char* String);

extern char* a_str_subGetRange(const char* String, int Start, int End);
extern char* a_str_subGetPrefix(const char* String, int Length);
extern char* a_str_subGetSuffix(const char* String, int Length);

extern int a_str_indexGetFirst(const char* String, char Character);
extern int a_str_indexGetLast(const char* String, char Character);

extern bool a_str_startsWith(const char* String, const char* Prefix);
extern bool a_str_endsWith(const char* String, const char* Suffix);

extern char* a_str_prefixGetToFirst(const char* String, char Marker);
extern char* a_str_prefixGetToLast(const char* String, char Marker);
extern char* a_str_suffixGetFromFirst(const char* String, char Marker);
extern char* a_str_suffixGetFromLast(const char* String, char Marker);

extern AList* a_str_split(const char* String, const char* Delimiters);

static inline bool a_str_equal(const char* String1, const char* String2)
{
    return strcmp(String1, String2) == 0;
}

#endif // A_INC_STRINGS_STR_P_H
