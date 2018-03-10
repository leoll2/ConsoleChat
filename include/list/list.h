#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list_chat_user.h"
#include "list_int.h"
#include "list_message.h"
#include "list_string.h"
#include "struct_util/struct_util.h"
#include "types.h"

/*Singly linked list functions for generic types*/
int ldatacmp(void* start, unsigned int size, void* pattern, bool indirect);
void ldatacpyval(list_node* elem, void* data, unsigned int size);
void ldatacpyptr(list_node* elem, void* data);
list list_new();
void list_print(list l, void (*f)(void *));
void list_add_head_by_value(list* l, void* data, unsigned int size);
void list_add_head_by_pointer(list* l, void* data);
void list_add_tail_by_value(list* l, void* data, unsigned int size);
void list_add_tail_by_pointer(list* l, void* data);
list_node* list_extract(list* l, unsigned int offset, unsigned int size, void* pattern, bool ind);
list_node* list_extract_head(list* l);
list_node* list_extract_tail(list* l);
bool list_remove(list* l, unsigned int offset, unsigned int size, void* pattern, bool ind);
list_node* list_find(list l, unsigned int offset, unsigned int size, void* pattern, bool ind);

#endif
