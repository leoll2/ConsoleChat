/* This file contains functions which deal with the server terminal,
 * that is major I/O operations.*/

#include "server.h"

/* Gets the command from the client and executes it.
 * Returns 0 if success, -1 otherwise.*/
int get_command(int sock) {
	int ret;
	uint16_t cmd;
	char* username = NULL;
	ret = get_cmd_from_cl(sock, &cmd, &username);
	if (ret == 0) {		//if no errors occurred while trying to read the command
		ret = sv_exec_command(sock, cmd, username);
		printf("Processed command '%s %s' from socket %d. Return value = %d\n", commands[cmd].name, (username ? username : ""), sock, ret);
	}
	if (username != NULL) //free the memory if used
		free(username);
	return ret;
}

/* Reads the port, specified as argument of the main program.
 * Returns the port number, or exits in case of error.*/
int parse_port(int argc, char* argv[]) {
	long parsed_value;
	if (argc != 2) {
		printf("Incorrect number of arguments. Correct syntax is <program> <sv_port>\n");
		exit(1);
	}
	parsed_value = strtol(argv[1], NULL, 10);
	if (!parsed_value) {
		printf("Bad argument (server port).\n");
		exit(1);
	}
	if (parsed_value < 1 || parsed_value > 65535) {
		printf("Port number must be in range [1, 65535].\n");
		exit(1);
	}
	return (uint16_t)parsed_value;
}