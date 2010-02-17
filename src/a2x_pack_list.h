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

#ifndef A2X_PACK_LIST_H
#define A2X_PACK_LIST_H

#include <stdlib.h>
#include <string.h>

typedef struct Node {
    void* content;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct List {
    Node* first;
    Node* last;
    Node* current;
    int items;
} List;

typedef struct ListIterator {
    List* list;
    Node* current;
} ListIterator;

#define a_list_first(l)       ((l)->first->content)
#define a_list_last(l)        ((l)->last->content)
#define a_list_current(l)     ((l)->current->content)
#define a_list_currentNode(l) ((l)->current)

#define a_list_setContent(n, c) ((n)->content = (c))

#define a_list_items(l)   ((l)->items)
#define a_list_size(l)    ((l)->items)
#define a_list_isEmpty(l) ((l)->first->next == (l)->last)

extern List* a_list_set(void);
extern void a_list_free(List* const list);
extern void a_list_freeContent(List* const list);

extern void a_list_empty(List* const list);
extern void a_list_emptyContent(List* const list);
extern Node* a_list_addFirst(List* const list, void* const content);
extern Node* a_list_addLast(List* const list, void* const content);
extern void a_list_reset(List* const list);
extern int a_list_iterate(List* const list);
extern void* a_list_pop(List* const list);
extern void* a_list_peek(List* const list);
extern void a_list_removeFirst(List* const list);
extern void a_list_removeFirstContent(List* const list);
extern void a_list_removeLast(List* const list);
extern void a_list_removeLastContent(List* const list);
extern void a_list_removeCurrent(List* const list);
extern void a_list_removeCurrentContent(List* const list);
extern void a_list_removeNode(Node* const node);
extern void a_list_removeNodeContent(Node* const node);
extern void* a_list_getArray(List* const list, const int size);

extern ListIterator a_list_iterator(List* const list);

#define a_list_iteratorNext(i)   \
({                               \
    i.current = i.current->next; \
    i.current != i.list->last;   \
})

#define a_list_iteratorGet(i) ((i).current->content)

#endif // A2X_PACK_LIST_H
