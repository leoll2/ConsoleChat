/*This file contains the parameters of the application. Some of them can be tuned
 to fit the needs.*/

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/*=============*/
/*ADJUSTABLE*/

//max # of connections queued for server accept()
#define BACKLOG_SIZE 32
//number of available commands
#define NUM_OF_COMMANDS 6
//max # of characters of the command (only keyword)
#define MAX_LENGTH_COMMAND 15
//max # of characters of usernames
#define MAX_LENGTH_USERNAME 20
//max # of characters accepted as single input
#define MAX_LENGTH_INPUT_STRING	(MAX_LENGTH_COMMAND + MAX_LENGTH_USERNAME + 1)
//max # of characters in a message
#define MAX_LENGTH_MESSAGE 1500
//max # of buffered incoming instant messages
#define IN_MSG_BUFFER_SIZE 30
//delay (s) between consecutive checks for instant messages
#define POLLING_TIME 2
//enable(1)/disable(0) command !who for non-registered users
#define ENABLE_UNREG_WHO 0

/*=============*/
/*DO NOT TOUCH!*/
//Command codes (from client to server)
#define CMD_HELP        0
#define CMD_REGISTER    1
#define CMD_DEREGISTER  2
#define CMD_WHO         3
#define CMD_SEND        4
#define CMD_QUIT        5
//Special commands (from client to server)
#define SEND_OK_OFFLINE 6
#define SEND_NO_OFFLINE 7
//Response codes (from server to client)
#define REG_DOUBLE_LOGIN    128
#define REG_NEW_USER        129
#define REG_RECONNECTED     130
#define WHO_EMPTY           131
#define WHO_READY           132
#define DEREG_FAILED        133
#define DEREG_DONE          134
#define SEND_NONEXIST_USER  135
#define SEND_ONLINE_USER    136
#define SEND_OFFLINE_USER   137
//Other
#define SOCK_UNNECESSARY    -20

#endif
