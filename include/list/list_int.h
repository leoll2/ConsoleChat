#ifndef LIST_INT_H_
#define LIST_INT_H_

//#include "list.h"

/*These functions implement linked lists for
 int data type.*/
list_int list_int_new();
void list_int_add_head(list_int* l, int value);
void list_int_add_tail(list_int* l, int value);
void print_int(void* d);
void println_int(void* d);
void list_int_print(list_int l);
void list_int_println(list_int l);
list_int_node* list_int_extract(list_int* l, int value);
list_int_node* list_int_extract_head(list_int* l);
list_int_node* list_int_extract_tail(list_int* l);
bool list_int_remove(list_int* l, int value);
list_int_node* list_int_find(list_int l, int value);

#endif
