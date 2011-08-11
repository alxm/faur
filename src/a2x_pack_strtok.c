/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_strtok.v.h"

struct StringTok {
    char* string;
    char* delims;
    int numDelims;
    int index;
    char* tok;
};

static bool is_delim(const char c, const char* const d, const int n);

StringTok* a_strtok_set(const char* const s, const char* const d)
{
    StringTok* const t = malloc(sizeof(StringTok));

    t->string = a_str_dup(s);
    t->delims = a_str_dup(d);
    t->numDelims = strlen(d);
    t->index = 0;
    t->tok = NULL;

    return t;
}

void a_strtok_free(StringTok* const t)
{
    free(t->string);
    free(t->delims);
    free(t->tok);

    free(t);
}

bool a_strtok_next(StringTok* const t)
{
    char* const string = t->string;
    const char* const delims = t->delims;
    const int numDelims = t->numDelims;

    int start = t->index;
    int end;

    while(1) {
        if(string[start] == '\0') {
            return false;
        } else if(is_delim(string[start], delims, numDelims)) {
            start++;
        } else {
            break;
        }
    }

    for(end = start; string[end] != '\0' && !is_delim(string[end], delims, numDelims); end++) {
        continue;
    }

    const int len = end - start;
    char* const str = malloc((len + 1) * sizeof(char));

    memcpy(str, &string[start], len);
    str[len] = '\0';

    t->index = end;

    free(t->tok);
    t->tok = str;

    return true;
}

char* a_strtok_get(StringTok* const t)
{
    return t->tok;
}

static bool is_delim(const char c, const char* const d, const int n)
{
    for(int i = n; i--; ) {
        if(c == d[i]) {
            return true;
        }
    }

    return false;
}
