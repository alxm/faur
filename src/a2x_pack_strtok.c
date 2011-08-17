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
};

static bool is_delim(const char c, const char* const d, const int n);

StringTok* a_strtok_new(const char* const s, const char* const d)
{
    StringTok* const t = malloc(sizeof(StringTok));

    t->string = a_str_dup(s);
    t->delims = a_str_dup(d);
    t->numDelims = strlen(d);
    t->index = 0;

    return t;
}

void a_strtok_free(StringTok* const t)
{
    free(t);
}

bool a_strtok_next(StringTok* const t)
{
    const char* const string = t->string;
    const char* const delims = t->delims;
    const int numDelims = t->numDelims;

    while(true) {
        if(string[t->index] == '\0') {
            return false;
        } else if(is_delim(string[t->index], delims, numDelims)) {
            t->index++;
        } else {
            return true;
        }
    }
}

char* a_strtok_get(StringTok* const t)
{
    const char* const string = t->string;
    const char* const delims = t->delims;
    const int numDelims = t->numDelims;

    while(is_delim(string[t->index], delims, numDelims)) {
        t->index++;
    }

    const int start = t->index;

    while(string[t->index] != '\0' && !is_delim(string[t->index], delims, numDelims)) {
        t->index++;
    }

    const int len = t->index - start;
    char* const token = a_str__alloc((len + 1) * sizeof(char));

    memcpy(token, &string[start], len);
    token[len] = '\0';

    return token;
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
