/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_STRINGS_STR_P_H
#define F_INC_STRINGS_STR_P_H

#include "../general/f_system_includes.h"

#include "../data/f_list.p.h"
#include "../general/f_errors.p.h"

extern bool f_str_fmt(char* Buffer, size_t Size, bool OverflowOk, const char* Format, ...) F__ATTRIBUTE_FORMAT(4);
extern bool f_str_fmtv(char* Buffer, size_t Size, bool OverflowOk, const char* Format, va_list Args);

extern char* f_str_merge(const char* String1, ...) F__ATTRIBUTE_FORMAT(1);
extern char* f_str_dup(const char* String);
extern char* f_str_trim(const char* String);

extern char* f_str_subGetRange(const char* String, int Start, int End);
extern char* f_str_subGetPrefix(const char* String, int Length);
extern char* f_str_subGetSuffix(const char* String, int Length);

extern int f_str_indexGetFirst(const char* String, char Character);
extern int f_str_indexGetLast(const char* String, char Character);

extern bool f_str_startsWith(const char* String, const char* Prefix);
extern bool f_str_endsWith(const char* String, const char* Suffix);

extern char* f_str_prefixGetToFirst(const char* String, char Marker);
extern char* f_str_prefixGetToLast(const char* String, char Marker);
extern char* f_str_suffixGetFromFirst(const char* String, char Marker);
extern char* f_str_suffixGetFromLast(const char* String, char Marker);

extern FList* f_str_split(const char* String, const char* Delimiters);
extern void f_str_splitFree(FList* List);

static inline bool f_str_equal(const char* String1, const char* String2)
{
    F_CHECK(String1 != NULL);
    F_CHECK(String2 != NULL);

    return strcmp(String1, String2) == 0;
}

#endif // F_INC_STRINGS_STR_P_H
