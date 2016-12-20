/*
    Copyright 2010, 2016 Alex Margarit

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

struct AStrTok {
    const char* string;
    const char* delims;
    int numDelims;
    int index;
    char* currentToken;
    int currentBufferLen;
};

static bool is_delim(char Character, const char* Delimiters, int NumDelimiters)
{
    for(int i = NumDelimiters; i--; ) {
        if(Character == Delimiters[i]) {
            return true;
        }
    }

    return false;
}

AStrTok* a_strtok_new(const char* String, const char* Delimiters)
{
    AStrTok* t = a_mem_malloc(sizeof(AStrTok));

    t->string = String;
    t->delims = Delimiters;
    t->numDelims = strlen(Delimiters);
    t->index = 0;
    t->currentToken = NULL;
    t->currentBufferLen = 0;

    return t;
}

void a_strtok_free(AStrTok* Tokenizer)
{
    if(Tokenizer->currentToken) {
        free(Tokenizer->currentToken);
    }

    free(Tokenizer);
}

char* a_strtok__get(AStrTok* Tokenizer)
{
    const char* const string = Tokenizer->string;
    const char* const delims = Tokenizer->delims;
    const int numDelims = Tokenizer->numDelims;

    while(is_delim(string[Tokenizer->index], delims, numDelims)) {
        Tokenizer->index++;
    }

    if(string[Tokenizer->index] == '\0') {
        // Reset index, in case A_STRTOK_ITERATE is called again
        Tokenizer->index = 0;
        return NULL;
    }

    const int start = Tokenizer->index;

    do {
        Tokenizer->index++;
    } while(string[Tokenizer->index] != '\0'
        && !is_delim(string[Tokenizer->index], delims, numDelims));

    const int len = Tokenizer->index - start;

    if(len > Tokenizer->currentBufferLen) {
        if(Tokenizer->currentToken) {
            free(Tokenizer->currentToken);
        }

        Tokenizer->currentToken = a_mem_malloc(len + 1);
        Tokenizer->currentBufferLen = len;
    }

    memcpy(Tokenizer->currentToken, &string[start], len);
    Tokenizer->currentToken[len] = '\0';

    return Tokenizer->currentToken;
}
