#ifndef CLIENT_H_
#define CLIENT_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "chat.h"
#include "commands.h"

/*GLOBAL VARIABLES*/
/*cl_console.c*/
extern struct termios oldt;	//old terminal config
extern struct termios newt;	//new terminal config
extern char in_buf[];		//input buffer
extern int in_buf_pointer;	//pointer of in_buf
/*cl_main.c*/
extern int sd;				//data socket to server
extern int udp_sock;		//socket for instant messages
extern uint16_t my_port;
extern struct in_addr my_ip;
extern bool logged;			//true if the client has registered, false otherwise
extern char* current_user;	//nickname of current user (if logged)
extern pthread_mutex_t console_mutex;

/*FUNCTIONS*/
/*cl_console.c*/
void adv_printf(char* msg);
uint16_t ask_to_send_offline(char* dest);
int get_command();
int get_message(char** msg, uint16_t* msg_len);
void parse_arguments(int argc, char* argv[], struct in_addr* ma, uint16_t* mp, struct in_addr* sa, uint16_t* sp);
void init_console_config();
void reset_console_config();

/*cl_exec.c*/
int cl_exec_help();
int cl_exec_register(char* username);
int cl_exec_deregister();
int cl_exec_who();
int cl_exec_send(char* dest_username);
int cl_exec_quit();
int cl_exec_command(uint16_t cmd_code, char* username);
void init_handlers();

/*cl_main.c*/

/*cl_network.c*/
int download_offline_msg();
int recv_dest_addr_from_sv(struct sockaddr_in* dest_addr);
int recv_response_from_sv(uint16_t* response_code);
int recv_udp_msg(char** msgbuf, char* recvbuf, int* next);
int recv_users_list_from_sv();
int reply_to_sv(uint16_t chatmode);
void show_connection(struct in_addr sa, int sp, int mp);
int send_addr_to_sv();
int send_cmd_to_sv(uint16_t cmd_code, char* username);
int send_msg_to_sv(char* msg, uint16_t msg_len);
int send_udp_msg(char* msg, struct sockaddr_in dest_addr);

#endif
