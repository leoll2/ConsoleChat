#include <stdio.h>
#include <string.h>
#include "list.h"

bool list_message_node_delete(list_message_node* p) {
    if (!p)
        return false;
    delete_message(p->data);
    free(p);
    return true;
}

list_message list_message_new() {
	return list_new();
}

void list_message_add_head(list_message* l, message* m) {
	list_add_head_by_pointer(l, (void*)m);
}

void list_message_add_tail(list_message* l, message* m) {
	list_add_tail_by_pointer(l, (void*)m);
}

/*Alias of list_message_add_tail*/
void list_message_add(list_message* l, message* m) {
	list_add_tail_by_pointer(l, (void*)m);
}

void print_message(void* d) {
	printf("%s%s>\n%s", ((message*)d)->sender, (((message*)d)->instant ? " (instant msg)" : ""), ((message*)d)->body_text);
}

void println_message(void* d) {
	printf("%s%s>\n%s\n", ((message*)d)->sender, (((message*)d)->instant ? " (instant msg)" : ""), ((message*)d)->body_text);
}

void list_message_print(list_message l) {
	list_print(l, print_message);
}

void list_message_println(list_message l) {
	list_print(l, println_message);
}

/*Extracts from the list the first message from the specified user.*/
list_message_node* list_message_extract(list_message* l, char* username) {
	return list_extract(l, (unsigned int)(uintptr_t)&((message*)NULL)->sender, strlen(username) + 1, (void*)username, true);
}

list_message_node* list_message_extract_head(list_message* l) {
	return list_extract_head(l);
}

list_message_node* list_message_extract_tail(list_message* l) {
	return list_extract_tail(l);
}

bool list_message_remove_all(list_message* l) {
    list_message_node *p;
    if (!l)
        return false;
    while ((p = list_extract_head(l)))
        list_message_node_delete(p);
    return true;
}

bool list_message_remove(list_message* l, char* username) {
    list_message_node *p= list_extract(l, (unsigned int)(uintptr_t)&((message*)NULL)->sender, strlen(username) + 1, (void*)username, true);
    return list_message_node_delete(p);
}

list_message_node* list_message_find(list_message l, char* username) {
	return list_find(l, (unsigned int)(uintptr_t)&((message*)NULL)->sender, strlen(username) + 1, (void*)username, true);
}
