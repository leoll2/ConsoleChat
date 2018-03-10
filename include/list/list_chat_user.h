#ifndef LIST_CHAT_USER_H_
#define LIST_CHAT_USER_H_

//#include "list.h"
#include "types.h"

/*These functions implement linked lists for
 chat_user data type.*/
bool list_chat_user_node_delete(list_chat_user_node* p);
list_int list_chat_user_new();
void list_chat_user_add_head(list_chat_user* l, chat_user* cu);
void list_chat_user_add_tail(list_chat_user* l, chat_user* cu);
void print_chat_user(void* d);
void println_chat_user(void* d);
void list_chat_user_print(list_chat_user l);
void list_chat_user_println(list_chat_user l);
list_chat_user_node* list_chat_user_extract(list_chat_user* l, char* username);
list_chat_user_node* list_chat_user_extract_head(list_chat_user* l);
list_chat_user_node* list_chat_user_extract_tail(list_chat_user* l);
bool list_chat_user_remove_all(list_chat_user* l);
bool list_chat_user_remove(list_chat_user* l, char* username);
list_chat_user_node* list_chat_user_find_by_name(list_chat_user l, char* username);
list_chat_user_node* list_chat_user_find_by_socket(list_chat_user l, int sockfd);

#endif
