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

#include "a2x_pack_str.v.h"

#define NULL_STRING "(null)"
#define STRING_POOL_SIZE 1024

static List* pools;
static StringPool* pool;

void a_str__init(void)
{
    pools = a_list_new();
    pool = a_strpool_new(STRING_POOL_SIZE);
    a_list_addLast(pools, pool);
}

void a_str__uninit(void)
{
    A_LIST_ITERATE(pools, StringPool, s) {
        a_strpool_free(s);
    }
    a_list_free(pools);
}

char* a_str__alloc(unsigned int size)
{
    char* const str = a_strpool_alloc(pool, size);

    if(str) {
        return str;
    }

    pool = a_strpool_new(a_math_max(STRING_POOL_SIZE, size));
    a_list_addLast(pools, pool);

    return a_strpool_alloc(pool, size);
}

void* a_str__malloc(int count, ...)
{
    va_list args;
    unsigned int size = 0;

    va_start(args, count);

    for(int i = count; i--; ) {
        char* const s = va_arg(args, char*);

        if(s) {
            size += strlen(s);
        } else {
            size += strlen(NULL_STRING);
        }
    }

    va_end(args);

    return a_str__alloc((size + 1) * sizeof(char));
}

char* a_str__merge(int count, ...)
{
    va_list args;
    int size = 0;

    va_start(args, count);

    for(int i = count; i--; ) {
        char* const s = va_arg(args, char*);

        if(s) {
            size += strlen(s);
        } else {
            size += strlen(NULL_STRING);
        }
    }

    va_end(args);

    char* const str = malloc((size + 1) * sizeof(char));
    str[0] = '\0';

    va_start(args, count);

    for(int i = count; i--; ) {
        char* const s = va_arg(args, char*);

        if(s) {
            strcat(str, s);
        } else {
            strcat(str, NULL_STRING);
        }
    }

    va_end(args);

    return str;
}

char* a_str_dup(const char* s)
{
    char* const d = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(d, s);

    return d;
}

char* a_str_sub(const char* s, int start, int end)
{
    const int len = end - start;
    char* const str = malloc((len + 1) * sizeof(char));

    memcpy(str, s + start, len);
    str[len] = '\0';

    return str;
}

char* a_str_prefix(const char* s, int len)
{
    return a_str_sub(s, 0, len);
}

char* a_str_suffix(const char* s, int len)
{
    const int sLen = strlen(s);
    return a_str_sub(s, sLen - len, sLen);
}

int a_str_firstIndex(const char* s, char c)
{
    const int len = strlen(s);

    for(int i = 0; i < len; i++) {
        if(s[i] == c) {
            return i;
        }
    }

    return -1;
}

int a_str_lastIndex(const char* s, char c)
{
    for(int i = strlen(s); i--; ) {
        if(s[i] == c) {
            return i;
        }
    }

    return -1;
}

char* a_str_getPrefixFirstFind(const char* s, char limit)
{
    const int index = a_str_firstIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, 0, index);
}

char* a_str_getPrefixLastFind(const char* s, char limit)
{
    const int index = a_str_lastIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, 0, index);
}

char* a_str_getSuffixFirstFind(const char* s, char limit)
{
    const int index = a_str_firstIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, index + 1, strlen(s));
}

char* a_str_getSuffixLastFind(const char* s, char limit)
{
    const int index = a_str_lastIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, index + 1, strlen(s));
}

char* a_str_extractPath(const char* s)
{
    char* const path = a_str_getPrefixLastFind(s, '/');

    if(path) {
        return path;
    } else {
        return a_str_dup(".");
    }
}

char* a_str_extractFile(const char* s)
{
    char* const c = a_str_getSuffixLastFind(s, '/');

    if(c) {
        return c;
    } else {
        return a_str_dup(s);
    }
}

char* a_str_extractName(const char* s)
{
    char* const file = a_str_extractFile(s);
    char* const name = a_str_getPrefixLastFind(file, '.');

    if(name) {
        free(file);
        return name;
    } else {
        return file;
    }
}

char* a_str_trim(const char* s)
{
    const int len = strlen(s);

    int start;
    int end;

    for(start = 0; start < len && isspace(s[start]); start++) {
        continue;
    }

    for(end = len - 1; end >= start && isspace(s[end]); end--) {
        continue;
    }

    return a_str_sub(s, start, end + 1);
}
