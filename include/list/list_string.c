#include <stdio.h>
#include <string.h>
#include "list.h"
//#include "list_string.h"

list_int list_string_new() {
	return list_new();
}

void list_string_add_head(list_string* l, char* s) {
	list_add_head_by_value(l, (void*)s, strlen(s) + 1);
}

void list_string_add_tail(list_string* l, char* s) {
	list_add_tail_by_value(l, (void*)s, strlen(s) + 1);
}

void print_string(void* d) {
	printf("%s", (char*)d);
}

void println_string(void* d) {
	printf("%s\n", (char*)d);
}

void list_string_print(list_string l) {
	list_print(l, print_string);
}

void list_string_println(list_string l) {
	list_print(l, println_string);
}

list_string_node* list_string_extract(list_string* l, char* s) {
	return list_extract(l, 0, strlen(s) + 1, (void*)s, false);
}

list_string_node* list_string_extract_head(list_string* l) {
	return list_extract_head(l);
}

list_string_node* list_string_extract_tail(list_string* l) {
	return list_extract_tail(l);
}

bool list_string_remove(list_string* l, char* s) {
	return list_remove(l, 0, strlen(s) + 1, (void*)s, false);
}

list_string_node* list_string_find(list_string l, char* s) {
	return list_find(l, 0, strlen(s) + 1, (void*)s, false);
}
