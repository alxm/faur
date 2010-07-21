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

#include "a2x_pack_str.p.h"
#include "a2x_pack_str.v.h"

struct StringTok {
    char* string;
    char* delims;
    int numDelims;
    int index;
    char* tok;
};

static int a_string__isDelim(const char c, const char* const d, const int n);

StringTok* a_str_makeTok(const char* const s, const char* const d)
{
    StringTok* const t = malloc(sizeof(StringTok));

    t->string = a_str_dup(s);
    t->delims = a_str_dup(d);
    t->numDelims = strlen(d);
    t->index = 0;
    t->tok = NULL;

    return t;
}

void a_str_freeTok(StringTok* const t)
{
    free(t->string);
    free(t->delims);
    free(t->tok);

    free(t);
}

int a_str_hasTok(StringTok* const t)
{
    char* const string = t->string;
    const char* const delims = t->delims;
    const int numDelims = t->numDelims;

    int start = t->index;
    int end;

    while(1) {
        if(string[start] == '\0') {
            return 0;
        } else if(a_string__isDelim(string[start], delims, numDelims)) {
            start++;
        } else {
            break;
        }
    }

    for(end = start; string[end] != '\0' && !a_string__isDelim(string[end], delims, numDelims); end++) {
        continue;
    }

    const int len = end - start;
    char* const str = malloc((len + 1) * sizeof(char));

    memcpy(str, &string[start], len);
    str[len] = '\0';

    t->index = end;

    free(t->tok);
    t->tok = str;

    return 1;
}

char* a_str_getTok(StringTok* const t)
{
    return t->tok;
}

void* a_str_malloc2(int count, ...)
{
    va_list ap;
    int size = 0;

    va_start(ap, count);

    for(int i = count; i--; ) {
        size += strlen(va_arg(ap, char*));
    }

    va_end(ap);

    return malloc((size + 1) * sizeof(char));
}

char* a_str_dup(const char* const s)
{
    char* const d = a_str_malloc(s);
    strcpy(d, s);

    return d;
}

char* a_str_merge(int count, ...)
{
    va_list ap;
    int size = 0;

    va_start(ap, count);

    for(int i = count; i--; ) {
        size += strlen(va_arg(ap, char*));
    }

    va_end(ap);

    char* const s = malloc((size + 1) * sizeof(char));
    s[0] = '\0';

    va_start(ap, count);

    for(int i = count; i--; ) {
        strcat(s, va_arg(ap, char*));
    }

    va_end(ap);

    return s;
}

char* a_str_sub(const char* const s, const int start, const int end)
{
    const int len = end - start;
    char* const str = malloc((len + 1) * sizeof(char));

    memcpy(str, s + start, len);
    str[len] = '\0';

    return str;
}

int a_str_firstIndex(const char* const s, const char c)
{
    const int len = strlen(s);

    for(int i = 0; i < len; i++) {
        if(s[i] == c) {
            return i;
        }
    }

    return -1;
}

int a_str_lastIndex(const char* const s, const char c)
{
    for(int i = strlen(s); i--; ) {
        if(s[i] == c) {
            return i;
        }
    }

    return -1;
}

char* a_str_getPrefixFirstFind(const char* const s, const char limit)
{
    const int index = a_str_firstIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, 0, index);
}

char* a_str_getPrefixLastFind(const char* const s, const char limit)
{
    const int index = a_str_lastIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, 0, index);
}

char* a_str_getSuffixFirstFind(const char* const s, const char limit)
{
    const int index = a_str_firstIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, index + 1, strlen(s));
}

char* a_str_getSuffixLastFind(const char* const s, const char limit)
{
    const int index = a_str_lastIndex(s, limit);

    if(index == -1) {
        return NULL;
    }

    return a_str_sub(s, index + 1, strlen(s));
}

char* a_str_extractPath(const char* const s)
{
    char* const c = a_str_getPrefixLastFind(s, '/');

    if(c) {
        return c;
    } else {
        return a_str_dup("./");
    }
}

char* a_str_extractFile(const char* const s)
{
    char* const c = a_str_getSuffixLastFind(s, '/');

    if(c) {
        return c;
    } else {
        return a_str_dup(s);
    }
}

char* a_str_extractName(const char* const s)
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

char* a_str_trim(const char* const s)
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

static int a_string__isDelim(const char c, const char* const d, const int n)
{
    for(int i = n; i--; ) {
        if(c == d[i]) {
            return 1;
        }
    }

    return 0;
}
