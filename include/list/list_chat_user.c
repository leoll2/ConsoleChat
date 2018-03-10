#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include "list.h"

bool list_chat_user_node_delete(list_chat_user_node* p) {
    if (!p)
        return false;
    delete_chat_user(p->data);
    free(p);
    return true;
}

list_int list_chat_user_new() {
	return list_new();
}

void list_chat_user_add_head(list_chat_user* l, chat_user* cu) {
	list_add_head_by_pointer(l, (void*)cu);
}

void list_chat_user_add_tail(list_chat_user* l, chat_user* cu) {
	list_add_tail_by_pointer(l, (void*)cu);
}

void print_chat_user(void* d) {
	printf("%s", ((chat_user*)d)->username);
}

void println_chat_user(void* d) {
	printf("%s\n", ((chat_user*)d)->username);
}

void list_chat_user_print(list_chat_user l) {
	list_print(l, print_chat_user);
}

void list_chat_user_println(list_chat_user l) {
	list_print(l, println_chat_user);
}

list_chat_user_node* list_chat_user_extract(list_chat_user* l, char* username) {
	return list_extract(l, (unsigned int)(uintptr_t)&((chat_user*)NULL)->username, strlen(username) + 1, (void*)username, true);
}

list_chat_user_node* list_chat_user_extract_head(list_chat_user* l) {
	return list_extract_head(l);
}

list_chat_user_node* list_chat_user_extract_tail(list_chat_user* l) {
	return list_extract_tail(l);
}

bool list_chat_user_remove_all(list_chat_user* l) {
    list_chat_user_node *p;
    if (!l)
        return false;
    while ((p = list_extract_head(l)))
        list_chat_user_node_delete(p);
    return true;
}

bool list_chat_user_remove(list_chat_user* l, char* username) {
    list_chat_user_node *p = list_extract(l, (unsigned int)(uintptr_t)&((chat_user*)NULL)->username, strlen(username) + 1, (void*)username, true);
    return list_chat_user_node_delete(p);
}
list_chat_user_node* list_chat_user_find_by_name(list_chat_user l, char* username) {
	return list_find(l, (unsigned int)(uintptr_t)&((chat_user*)NULL)->username, strlen(username) + 1, (void*)username, true);
}

list_chat_user_node* list_chat_user_find_by_socket(list_chat_user l, int sockfd) {
    return list_find(l, (unsigned int)(uintptr_t)&((chat_user*)NULL)->sock, sizeof(int), (void*)&sockfd, false);
}
