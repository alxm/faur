/*
    Copyright 2017, 2018 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_ecs_message.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

AMessage* a_message__new(AEntity* To, AEntity* From, const char* Message)
{
    AMessage* m = a_mem_malloc(sizeof(AMessage));

    m->to = To;
    m->from = From;
    m->message = a_str_dup(Message);

    a_entity_refInc(To);
    a_entity_refInc(From);

    return m;
}

void a_message__free(AMessage* Message)
{
    a_entity_refDec(Message->to);
    a_entity_refDec(Message->from);

    free(Message->message);
    free(Message);
}

void a_message_handlerSet(AEntity* Entity, const char* Message, AMessageHandler* Handler, bool HandleImmediately)
{
    if(a_strhash_contains(Entity->handlers, Message)) {
        a_out__fatal("'%s' handler already set for '%s'",
                     Message,
                     a_entity_idGet(Entity));
    }

    AMessageHandlerContainer* h = a_mem_malloc(sizeof(AMessageHandlerContainer));

    h->handler = Handler;
    h->handleImmediately = HandleImmediately;

    a_strhash_add(Entity->handlers, Message, h);
}

void a_message_send(AEntity* To, AEntity* From, const char* Message)
{
    AMessageHandlerContainer* h = a_strhash_get(To->handlers, Message);

    if(h == NULL) {
        return;
    }

    if(h->handleImmediately) {
        if(!a_entity_removeGet(To) && !a_entity_removeGet(From)
            && !a_entity_muteGet(To)) {

            h->handler(To, From);
        }
    } else {
        a_ecs__queueMessage(To, From, Message);
    }
}
