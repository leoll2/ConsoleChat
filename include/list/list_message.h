#ifndef LIST_MESSAGE_H
#define LIST_MESSAGE_H

#include "types.h"

/*These functions implement linked lists for
 message data type.*/
bool list_message_node_delete(list_message_node* p);
list_message list_message_new();
void list_message_add_head(list_message* l, message* m);
void list_message_add_tail(list_message* l, message* m);
void list_message_add(list_message* l, message* m);
void print_message(void* d);
void println_message(void* d);
void list_message_print(list_message l);
void list_message_println(list_message l);
list_message_node* list_message_extract(list_message* l, char* username);
list_message_node* list_message_extract_head(list_message* l);
list_message_node* list_message_extract_tail(list_message* l);
bool list_message_remove_all(list_message* l);
bool list_message_remove(list_message* l, char* username);
list_message_node* list_message_find(list_message l, char* username);

#endif
