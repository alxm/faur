/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_fps.v.h"
#include "a2x_pack_input_controller.v.h"
#include "a2x_pack_input_touch.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_str.v.h"

typedef struct {
    AInputCallback callback;
} AInputCallbackContainer;

static AList* g_callbacks;

void a_input__init(void)
{
    g_callbacks = a_list_new();

    a_input_button__init();
    a_input_controller__init();
}

void a_input__uninit(void)
{
    a_input_button__uninit();

    a_list_freeEx(g_callbacks, free);
}

void a_input__callbackAdd(AInputCallback Callback)
{
    AInputCallbackContainer* c = a_mem_malloc(sizeof(AInputCallbackContainer));
    c->callback = Callback;

    a_list_addLast(g_callbacks, c);
}

void a_input__userHeaderInit(AInputUserHeader* Header)
{
    Header->name = NULL;
    Header->sourceInputs = a_list_new();
    Header->platformInputs = a_list_new();
}

void a_input__userHeaderFree(AInputUserHeader* Header)
{
    free(Header->name);

    a_list_free(Header->sourceInputs);
    a_list_free(Header->platformInputs);
}

void a_input__sourceHeaderInit(AInputSourceHeader* Header, const char* Name)
{
    Header->name = a_str_dup(Name);
    Header->lastEventFrame = 0;
}

void a_input__sourceHeaderFree(AInputSourceHeader* Header)
{
    free(Header->name);
    free(Header);
}

void a_input__userHeaderFindSource(AInputUserHeader* UserInput, const char* Id, const AStrHash* GlobalCollection, const AStrHash* ControllerCollection)
{
    AInputSourceHeader* source = NULL;

    if(GlobalCollection) {
        source = a_strhash_get(GlobalCollection, Id);
    }

    if(source == NULL && ControllerCollection) {
        source = a_strhash_get(ControllerCollection, Id);
    }

    if(source) {
        a_list_addLast(UserInput->sourceInputs, source);
    }
}

bool a_input__freshEventGet(const AInputSourceHeader* Header)
{
    return Header->lastEventFrame == a_fps_ticksGet();
}

void a_input__freshEventSet(AInputSourceHeader* Header)
{
    Header->lastEventFrame = a_fps_ticksGet();
}

void a_input__tick(void)
{
    a_platform__inputsPoll();
    a_input_button__tick();

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        c->callback();
    }
}
