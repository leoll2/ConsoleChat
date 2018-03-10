/*This file contains the definitions of custom types.*/

#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*Generic*/
typedef struct list_node {
	void* data;
	struct list_node* next;
} list_node;
typedef list_node* list;

/*Functions*/
typedef int (*generic_fp)(void);
typedef int (*oneargcp_fp)(char*);
typedef int (*oneargi_fp)(int);
typedef int (*twoargicp_fp)(int, char*);

/*Int*/
typedef list list_int;
typedef list_node list_int_node;

/*String*/
typedef list list_string;
typedef list_node list_string_node;

/*Message*/
typedef struct message {
	char* sender;
	char* body_text;
	bool instant;
} message;
typedef list list_message;
typedef list_node list_message_node;

/*Chat_user*/
typedef struct chat_user {
	char* username;
    int sock;
	struct sockaddr_in full_address;
    bool online;
	list_message pending_msg;
} chat_user;
typedef list list_chat_user;
typedef list_node list_chat_user_node;

/*Command*/
typedef struct command {
	char* name;
	char* description;
    uint16_t code;
	bool arg;
	generic_fp handler;
} command;

#endif
