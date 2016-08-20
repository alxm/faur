/*
    Copyright 2010 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

#define a_str_equal(String1, String2) (strcmp(String1, String2) == 0)

#define a_str_malloc(...)                                               \
({                                                                      \
    const char* const a__strs[] = {__VA_ARGS__};                        \
    a_str__malloc((sizeof(a__strs) / sizeof(a__strs[0])), __VA_ARGS__); \
})

#define a_str_merge(...)                                               \
({                                                                     \
    const char* const a__strs[] = {__VA_ARGS__};                       \
    a_str__merge((sizeof(a__strs) / sizeof(a__strs[0])), __VA_ARGS__); \
})

extern void* a_str__malloc(int Count, ...);
extern char* a_str__merge(int Count, ...);

extern char* a_str_dup(const char* String);

extern char* a_str_sub(const char* String, int Start, int End);
extern char* a_str_prefix(const char* String, int Length);
extern char* a_str_suffix(const char* String, int Length);

extern int a_str_firstIndex(const char* String, char Character);
extern int a_str_lastIndex(const char* String, char Character);

extern char* a_str_getPrefixFirstFind(const char* String, char Marker);
extern char* a_str_getPrefixLastFind(const char* String, char Marker);
extern char* a_str_getSuffixFirstFind(const char* String, char Marker);
extern char* a_str_getSuffixLastFind(const char* String, char Marker);

extern char* a_str_extractPath(const char* String);
extern char* a_str_extractFile(const char* String);
extern char* a_str_extractName(const char* String);

extern char* a_str_trim(const char* String);
