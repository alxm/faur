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

void* a_str__malloc(int Count, ...)
{
    va_list args;
    unsigned int size = 0;

    va_start(args, Count);

    for(int i = Count; i--; ) {
        char* const s = va_arg(args, char*);

        if(s) {
            size += strlen(s);
        } else {
            size += strlen(NULL_STRING);
        }
    }

    va_end(args);

    return a_mem_malloc((size + 1) * sizeof(char));
}

char* a_str__merge(int Count, ...)
{
    va_list args;
    int size = 0;

    va_start(args, Count);

    for(int i = Count; i--; ) {
        char* const s = va_arg(args, char*);

        if(s) {
            size += strlen(s);
        } else {
            size += strlen(NULL_STRING);
        }
    }

    va_end(args);

    char* const str = a_mem_malloc((size + 1) * sizeof(char));
    str[0] = '\0';

    va_start(args, Count);

    for(int i = Count; i--; ) {
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

char* a_str_dup(const char* String)
{
    char* buffer = a_mem_malloc((strlen(String) + 1) * sizeof(char));
    return strcpy(buffer, String);
}

char* a_str_sub(const char* String, int Start, int End)
{
    const int len = End - Start;
    char* const str = a_mem_malloc((len + 1) * sizeof(char));

    memcpy(str, String + Start, len);
    str[len] = '\0';

    return str;
}

char* a_str_prefix(const char* String, int Length)
{
    return a_str_sub(String, 0, Length);
}

char* a_str_suffix(const char* String, int Length)
{
    const int sLen = strlen(String);
    return a_str_sub(String, sLen - Length, sLen);
}

int a_str_firstIndex(const char* String, char Character)
{
    for(int i = 0; String[i] != '\0'; i++) {
        if(String[i] == Character) {
            return i;
        }
    }

    return -1;
}

int a_str_lastIndex(const char* String, char Character)
{
    for(int i = strlen(String); i--; ) {
        if(String[i] == Character) {
            return i;
        }
    }

    return -1;
}

char* a_str_getPrefixFirstFind(const char* String, char Marker)
{
    const int index = a_str_firstIndex(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(String, 0, index);
}

char* a_str_getPrefixLastFind(const char* String, char Marker)
{
    const int index = a_str_lastIndex(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(String, 0, index);
}

char* a_str_getSuffixFirstFind(const char* String, char Marker)
{
    const int start = a_str_firstIndex(String, Marker) + 1;
    int end = start;

    if(start == 0) {
        return NULL;
    }

    while(String[end] != '\0') {
        end++;
    }

    return a_str_sub(String, start, end);
}

char* a_str_getSuffixLastFind(const char* String, char Marker)
{
    const int index = a_str_lastIndex(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(String, index + 1, strlen(String));
}

char* a_str_extractPath(const char* String)
{
    char* const path = a_str_getPrefixLastFind(String, '/');

    if(path) {
        return path;
    } else {
        return a_str_dup(".");
    }
}

char* a_str_extractFile(const char* String)
{
    char* const c = a_str_getSuffixLastFind(String, '/');

    if(c) {
        return c;
    } else {
        return a_str_dup(String);
    }
}

char* a_str_extractName(const char* String)
{
    char* const file = a_str_extractFile(String);
    char* const name = a_str_getPrefixLastFind(file, '.');

    if(name) {
        free(file);
        return name;
    } else {
        return file;
    }
}

char* a_str_trim(const char* String)
{
    int start, end;

    for(start = 0; isspace(String[start]); start++) {
        continue;
    }

    for(end = strlen(String) - 1; end >= start && isspace(String[end]); end--) {
        continue;
    }

    return a_str_sub(String, start, end + 1);
}
