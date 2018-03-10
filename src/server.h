#ifndef SERVER_H_
#define SERVER_H_

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "chat.h"
#include "commands.h"

/*VARIABLES*/
/*sv_main.c*/
extern list_chat_user lcu;		//list of registered chat users
extern fd_set master_fds;		//main set of file descriptors

/*FUNCTIONS*/

/*sv_console.c*/
int get_command();
int parse_port(int argc, char* argv[]);

/*sv_exec.c*/
void init_handlers();
int store_message(char* sender, char* dest, char* body);
int sv_exec_command(int sock, uint16_t cmd_code, char* username);
int sv_exec_help(int sock);
int sv_exec_register(int sock, char* username);
int sv_exec_deregister(int sock);
int sv_exec_who(int sock);
int sv_exec_send(int sock, char* dest_name);
int sv_exec_quit(int sock);
/*sv_main.c*/

/*sv_network.c*/
void close_sock_clear_set(int sock);
int get_cmd_from_cl(int sock, uint16_t* cmd, char** username);
int read_message(int sock, char* sender_name, char* dest_name);
int request_addr_from_cl(int sock, struct in_addr* ip, uint16_t* port);
int send_dest_addr_to_cl(int sock, struct sockaddr_in full_addr);
int send_response_to_cl(int sock, uint16_t response_code);
int send_offline_msg(int sock, message* msg);
int send_offline_messages_to_cl(int sock, list_chat_user_node* user_node);
int send_users_list_to_cl(int sock);


#endif
