#ifndef LIST_STRING_H_
#define LIST_STRING_H_

//#include "list.h"

/*These functions implement linked lists for
 strings (char*) data type.*/
list_int list_string_new();
void list_string_add_head(list_string* l, char* s);
void list_string_add_tail(list_string* l, char* s);
void print_string(void* d);
void println_string(void* d);
void list_string_print(list_string l);
void list_string_println(list_string l);
list_string_node* list_string_extract(list_string* l, char* s);
list_string_node* list_string_extract_head(list_string* l);
list_string_node* list_string_extract_tail(list_string* l);
bool list_string_remove(list_string* l, char* s);
list_string_node* list_string_find(list_string l, char* s);

#endif
