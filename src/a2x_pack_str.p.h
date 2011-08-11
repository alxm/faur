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

#ifndef A2X_PACK_STR_PH
#define A2X_PACK_STR_PH

#include "a2x_app_includes.h"

#define a_str_same(s1, s2)  (strcmp((s1), (s2)) == 0)
#define a_str_equal(s1, s2) (strcmp((s1), (s2)) == 0)
#define a_str_size(s)       ((strlen(s) + 1) * sizeof(char))

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

extern void* a_str__malloc(int count, ...);
extern char* a_str__merge(int count, ...);

extern char* a_str_dup(const char* const s);

extern char* a_str_sub(const char* const s, const int start, const int end);

extern int a_str_firstIndex(const char* const s, const char c);
extern int a_str_lastIndex(const char* const s, const char c);

extern char* a_str_getPrefixFirstFind(const char* const s, const char limit);
extern char* a_str_getPrefixLastFind(const char* const s, const char limit);
extern char* a_str_getSuffixFirstFind(const char* const s, const char limit);
extern char* a_str_getSuffixLastFind(const char* const s, const char limit);

extern char* a_str_extractPath(const char* const s);
extern char* a_str_extractFile(const char* const s);
extern char* a_str_extractName(const char* const s);

extern char* a_str_trim(const char* const s);

#endif // A2X_PACK_STR_PH
