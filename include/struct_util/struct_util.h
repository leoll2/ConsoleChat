#ifndef STRUCT_UTIL_H
#define STRUCT_UTIL_H


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include "list/list.h"
#include "parameters.h"
#include "types.h"

/*Message*/
message* new_message(char* sender, char* content, bool instant);
bool delete_message(message* m);

/*Sockaddr_in*/
struct sockaddr_in build_sockaddr_in(char* ip, unsigned short port);
struct sockaddr_in build_sockaddr_in2(struct in_addr ip, unsigned short port);

/*Chat_user*/
bool delete_chat_user(chat_user* cu);
chat_user* new_chat_user(char *name, int sockfd, struct sockaddr_in full_addr);
chat_user* new_chat_user2(char *name, int sockfd, struct in_addr ip, unsigned short port);
chat_user* new_chat_user3(char *name, int sockfd, char* ip, unsigned short port);

#endif
