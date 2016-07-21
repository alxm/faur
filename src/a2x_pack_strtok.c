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

#include "a2x_pack_strtok.v.h"

struct AStringTok {
    const char* string;
    const char* delims;
    int numDelims;
    int index;
    char* currentToken;
    int currentBufferLen;
};

static bool is_delim(char c, const char* d, int n)
{
    for(int i = n; i--; ) {
        if(c == d[i]) {
            return true;
        }
    }

    return false;
}

AStringTok* a_strtok_new(const char* string, const char* delims)
{
    AStringTok* t = a_mem_malloc(sizeof(AStringTok));

    t->string = string;
    t->delims = delims;
    t->numDelims = strlen(delims);
    t->index = 0;
    t->currentToken = NULL;
    t->currentBufferLen = 0;

    return t;
}

void a_strtok_free(AStringTok* t)
{
    if(t->currentToken) {
        free(t->currentToken);
    }

    free(t);
}

char* a_strtok__get(AStringTok* t)
{
    const char* const string = t->string;
    const char* const delims = t->delims;
    const int numDelims = t->numDelims;

    while(is_delim(string[t->index], delims, numDelims)) {
        t->index++;
    }

    if(string[t->index] == '\0') {
        return NULL;
    }

    const int start = t->index;

    do {
        t->index++;
    } while(string[t->index] != '\0' && !is_delim(string[t->index], delims, numDelims));

    const int len = t->index - start;

    if(len > t->currentBufferLen) {
        if(t->currentToken) {
            free(t->currentToken);
        }

        t->currentToken = a_mem_malloc((len + 1) * sizeof(char));
        t->currentBufferLen = len;
    }

    memcpy(t->currentToken, &string[start], len);
    t->currentToken[len] = '\0';

    return t->currentToken;
}
