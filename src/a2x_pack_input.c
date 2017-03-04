/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_input.v.h"

typedef struct AInputCallbackContainer {
    AInputCallback callback;
} AInputCallbackContainer;

static AList* g_callbacks;

void a_input__init(void)
{
    g_callbacks = a_list_new();

    a_input_controller__init();
    a_input_analog__init();
    a_input_button__init();
    a_input_touch__init();

    a_sdl_input__bind();

    a_input_controller__init2();
    a_input_button__init2();
}

void a_input__uninit(void)
{
    a_input_controller__uninit();
    a_input_analog__uninit();
    a_input_button__uninit();
    a_input_touch__uninit();

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        free(c);
    }

    a_list_free(g_callbacks);
}

void a_input__addCallback(AInputCallback Callback)
{
    AInputCallbackContainer* c = a_mem_malloc(sizeof(AInputCallbackContainer));
    c->callback = Callback;

    a_list_addLast(g_callbacks, c);
}

void a_input__initHeader(AInputHeader* Header)
{
    Header->name = NULL;
    Header->sourceInputs = a_list_new();
}

void a_input__freeHeader(AInputHeader* Header)
{
    free(Header->name);
    a_list_free(Header->sourceInputs);
}

void a_input__initSourceHeader(AInputSourceHeader* Header, const char* Name)
{
    Header->name = a_str_dup(Name);
    Header->shortName = a_str_getSuffixLastFind(Name, '.');
    Header->lastEventFrame = 0;

    if(Header->shortName == NULL) {
        Header->shortName = a_str_dup(Name);
    }
}

void a_input__freeSourceHeader(AInputSourceHeader* Header)
{
    free(Header->name);
    free(Header->shortName);
    free(Header);
}

void a_input__findSourceInput(const char* Name, const AStrHash* GlobalCollection, const AStrHash* ControllerCollection, AInputHeader* UserInput)
{
    AInputSourceHeader* source = NULL;

    if(GlobalCollection) {
        source = a_strhash_get(GlobalCollection, Name);
    }

    if(source == NULL && ControllerCollection) {
        source = a_strhash_get(ControllerCollection, Name);
    }

    if(source != NULL) {
        if(UserInput->name == NULL) {
            UserInput->name = a_str_dup(source->shortName);
        }

        a_list_addLast(UserInput->sourceInputs, source);
    }
}

bool a_input__hasFreshEvent(const AInputSourceHeader* Header)
{
    return Header->lastEventFrame == a_fps_getCounter();
}

void a_input__setFreshEvent(AInputSourceHeader* Header)
{
    Header->lastEventFrame = a_fps_getCounter();
}

void a_input__get(void)
{
    a_input_touch__clearMotion();

    a_sdl_input__get();

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        c->callback();
    }
}
