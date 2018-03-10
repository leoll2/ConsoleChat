#include <stdio.h>
#include "list.h"
//#include "list_int.h"

list_int list_int_new() {
	return list_new();
}

void list_int_add_head(list_int* l, int value) {
	list_add_head_by_value(l, (void*)&value, sizeof(int));
}

void list_int_add_tail(list_int* l, int value) {
	list_add_tail_by_value(l, (void*)&value, sizeof(int));
}

void print_int(void* d) {
	printf("%d", *(int *)d);
}

void println_int(void* d) {
	printf("%d\n", *(int *)d);
}

void list_int_print(list_int l) {
	list_print(l, print_int);
}

void list_int_println(list_int l) {
	list_print(l, println_int);
}

list_int_node* list_int_extract(list_int* l, int value) {
	return list_extract(l, 0, sizeof(int), (void*)&value, false);
}

list_int_node* list_int_extract_head(list_int* l) {
	return list_extract_head(l);
}

list_int_node* list_int_extract_tail(list_int* l) {
	return list_extract_tail(l);
}

bool list_int_remove(list_int* l, int value) {
	return list_remove(l, 0, sizeof(int), (void*)&value, false);
}

list_int_node* list_int_find(list_int l, int value) {
	return list_find(l, 0, sizeof(int), (void*)&value, false);
}
