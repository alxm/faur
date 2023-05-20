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

#include "f_str.v.h"
#include <faur.v.h>

static inline bool strFmtv(char* Buffer, size_t Size, bool OverflowOk, const char* Format, va_list Args)
{
    int r = vsnprintf(Buffer, Size, Format, Args);

    return r >= 0 && (r < (int)Size || OverflowOk);
}

bool f_str_fmt(char* Buffer, size_t Size, bool OverflowOk, const char* Format, ...)
{
    F__CHECK(Buffer != NULL);
    F__CHECK(Size > 0);
    F__CHECK(Format != NULL);

    va_list args;
    va_start(args, Format);

    bool ret = strFmtv(Buffer, Size, OverflowOk, Format, args);

    va_end(args);

    return ret;
}

bool f_str_fmtv(char* Buffer, size_t Size, bool OverflowOk, const char* Format, va_list Args)
{
    F__CHECK(Buffer != NULL);
    F__CHECK(Size > 0);
    F__CHECK(Format != NULL);

    return strFmtv(Buffer, Size, OverflowOk, Format, Args);
}

char* f_str_merge(const char* String1, ...)
{
    F__CHECK(String1 != NULL);

    va_list args;
    size_t size = 0;

    va_start(args, String1);

    for(const char* s = String1; s != NULL; s = va_arg(args, const char*)) {
        size += strlen(s);
    }

    va_end(args);

    char* string = f_mem_malloc(size + 1);
    char* buffer = string;

    va_start(args, String1);

    for(const char* s = String1; s != NULL; s = va_arg(args, const char*)) {
        while(*s != '\0') {
            *buffer++ = *s++;
        }
    }

    *buffer = '\0';

    va_end(args);

    return string;
}

char* f_str_dup(const char* String)
{
    if(String == NULL) {
        return NULL;
    }

    char* buffer = f_mem_malloc(strlen(String) + 1);

    return strcpy(buffer, String);
}

char* f_str_trim(const char* String)
{
    F__CHECK(String != NULL);

    int start = 0;
    int end = (int)strlen(String) - 1;

    while(isspace((int)String[start])) {
        start++;
    }

    while(end > start && isspace((int)String[end])) {
        end--;
    }

    return f_str_subGetRange(String, start, end + 1);
}

char* f_str_subGetRange(const char* String, int Start, int End)
{
    F__CHECK(String != NULL);
    F__CHECK(Start >= 0);
    F__CHECK(Start <= End);

    size_t len = (size_t)(End - Start);
    char* str = f_mem_malloc(len + 1);

    memcpy(str, String + Start, len);
    str[len] = '\0';

    return str;
}

char* f_str_subGetPrefix(const char* String, int Length)
{
    F__CHECK(String != NULL);
    F__CHECK(Length >= 0);

    return f_str_subGetRange(String, 0, Length);
}

char* f_str_subGetSuffix(const char* String, int Length)
{
    F__CHECK(String != NULL);
    F__CHECK(Length >= 0);

    int sLen = (int)strlen(String);

    return f_str_subGetRange(String, sLen - Length, sLen);
}

int f_str_indexGetFirst(const char* String, char Character)
{
    F__CHECK(String != NULL);

    for(int i = 0; String[i] != '\0'; i++) {
        if(String[i] == Character) {
            return i;
        }
    }

    return -1;
}

int f_str_indexGetLast(const char* String, char Character)
{
    F__CHECK(String != NULL);

    for(int i = (int)strlen(String); i--; ) {
        if(String[i] == Character) {
            return i;
        }
    }

    return -1;
}

bool f_str_startsWith(const char* String, const char* Prefix)
{
    F__CHECK(String != NULL);
    F__CHECK(Prefix != NULL);

    while(*String != '\0' && *Prefix != '\0') {
        if(*String++ != *Prefix++) {
            return false;
        }
    }

    return *Prefix == '\0';
}

bool f_str_endsWith(const char* String, const char* Suffix)
{
    F__CHECK(String != NULL);
    F__CHECK(Suffix != NULL);

    const size_t str_len = strlen(String);
    const size_t suf_len = strlen(Suffix);

    if(suf_len > str_len) {
        return false;
    }

    return strcmp(String + str_len - suf_len, Suffix) == 0;
}

char* f_str_prefixGetToFirst(const char* String, char Marker)
{
    F__CHECK(String != NULL);

    const int index = f_str_indexGetFirst(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return f_str_subGetRange(String, 0, index);
}

char* f_str_prefixGetToLast(const char* String, char Marker)
{
    F__CHECK(String != NULL);

    const int index = f_str_indexGetLast(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return f_str_subGetRange(String, 0, index);
}

char* f_str_suffixGetFromFirst(const char* String, char Marker)
{
    F__CHECK(String != NULL);

    const int start = f_str_indexGetFirst(String, Marker) + 1;
    int end = start;

    if(start == 0) {
        return NULL;
    }

    while(String[end] != '\0') {
        end++;
    }

    return f_str_subGetRange(String, start, end);
}

char* f_str_suffixGetFromLast(const char* String, char Marker)
{
    F__CHECK(String != NULL);

    const int index = f_str_indexGetLast(String, Marker);

    if(index == -1) {
        return NULL;
    }

    return f_str_subGetRange(String, index + 1, (int)strlen(String));
}

FList* f_str_split(const char* String, const char* Delimiters)
{
    F__CHECK(String != NULL);
    F__CHECK(Delimiters != NULL);

    FList* strings = f_list_new();

    const char* str = String;
    const char* start = String;

    while(*str != '\0') {
        for(const char* d = Delimiters; *d != '\0'; d++) {
            if(*str == *d) {
                if(str > start) {
                    size_t len = (size_t)(str - start);
                    char* split = f_mem_malloc(len + 1);

                    memcpy(split, start, len);
                    split[len] = '\0';

                    f_list_addLast(strings, split);
                }

                start = str + 1;
                break;
            }
        }

        str++;
    }

    if(str > start) {
        size_t len = (size_t)(str - start);
        char* split = f_mem_malloc(len + 1);

        memcpy(split, start, len);
        split[len] = '\0';

        f_list_addLast(strings, split);
    }

    return strings;
}

void f_str_splitFree(FList* List)
{
    if(List == NULL) {
        return;
    }

    f_list_freeEx(List, f_mem_free);
}
