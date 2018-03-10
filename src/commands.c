#include "commands.h"

/* Table of all available commands and their attributes.
 * Command | Description | Code | Arguments | Handler
 * The handler is initialized to null, then specified 
 * by modules that include this file.*/
command commands[NUM_OF_COMMANDS] = {
	{"!help", "show the list of available commands", CMD_HELP, false, NULL},
	{"!register", "register the client on the server", CMD_REGISTER, true, NULL},
	{"!deregister", "unregister the client from the server", CMD_DEREGISTER, false, NULL},
	{"!who", "show the list of registered users", CMD_WHO, false, NULL},
	{"!send", "send a message to another user", CMD_SEND, true, NULL},
	{"!quit", "log out and close the application", CMD_QUIT, false, NULL}
};

/* Executes (the handler of) a command.
 * Returns 0 if success, -1 otherwise.*/
int exec_comm(int sock, uint16_t cmd_code, char* user) {
	if (cmd_code >= NUM_OF_COMMANDS) {
		printf("Error: attempted to execute a non-existing command.\n");
		return -1;
	}
	if (sock == SOCK_UNNECESSARY)	//if socket isn't specified (i.e. client)
		return (commands[cmd_code].arg ? TO_1ARGCP(commands[cmd_code].handler, user) : commands[cmd_code].handler());
	else				//if socket is specified (i.e. server)
		return (commands[cmd_code].arg ? TO_2ARGICP(commands[cmd_code].handler, sock, user) : TO_1ARGI(commands[cmd_code].handler, sock));
}

/* Returns the command code given its code, -1 if the
 * command doesn't exist.*/
uint16_t get_cmd_code(char* cmd) {
	for (int i=0; i<NUM_OF_COMMANDS; ++i) {
		if (!strcmp(cmd, commands[i].name))
			return (commands[i].code);
	}
	return -1;
}

/* Returns true if the command needs an argument,
 * false otherwise.*/
int get_num_args(uint16_t cmd_code) {
	if (cmd_code >= NUM_OF_COMMANDS) {
		return -1;
	}
	return (commands[cmd_code].arg ? 1 : 0);
}

/* Sets the handler of the specified command.*/
int set_handler(uint16_t cmd_code, generic_fp fun) {
	if (cmd_code >= NUM_OF_COMMANDS) {
		printf("Error: attempted to define the behaviour of a non-existing command.\n");
		return -1;
	}
	commands[cmd_code].handler = fun;
	return 0;
}

/* Print all the available commands.*/
void show_commands() {
	printf("The following commands are available:\n");
	for (int i=0; i<NUM_OF_COMMANDS; ++i) {
		printf("%s %s --> %s\n", commands[i].name, (commands[i].arg ? "username" : ""), commands[i].description);
	}
	printf("\n");
}