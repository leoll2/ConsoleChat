/* This file contains functions which deal with the console,
 * for both input and output operations.*/

#include "client.h"

struct termios oldt, newt;	//structures to save the configurations of the console
char in_buf[MAX_LENGTH_INPUT_STRING + 3];	//user input buffer
int in_buf_pointer;			//index of the next available location in the buffer

/* A slightly modified printf(), adapted to work with
 * raw input. It clears the current line before printing.*/
void adv_printf(char* msg) {
	reset_line(MAX_LENGTH_INPUT_STRING);
	if (logged)
		printf("%s\n%s> %s", msg, current_user, in_buf);
	else
		printf("%s\n> %s", msg, in_buf);
	fflush(stdout);
}

/* When the recipient of a message is offline, the sender
 * is asked to send an offline message.
 * Returns the answer of the client.*/
uint16_t ask_to_send_offline(char* dest) {
	char c;
	printf("User %s is offline, do you want to send an offline message? (Y/N)\n", dest);
	c = getchar();
	switch(c) {
		case 'y':
		case 'Y':
			return SEND_OK_OFFLINE;
		case 'n':
		case 'N':
			return SEND_NO_OFFLINE;
		default:
			printf("\nInvalid input.");
			return SEND_NO_OFFLINE;
	}
}

/* Parses a command and, if valid, executes it.
 * Returns the same of the corresponding exec
 * function, -1 if an error occurred earlier.*/
int get_command() {
	char comm_name[MAX_LENGTH_COMMAND + 1],
		user_name[MAX_LENGTH_USERNAME + 1],
		unexpected_arg[MAX_LENGTH_COMMAND - 1];	//used to detect excess arguments
	int ret;
	uint16_t cmd_code;
	
	in_buf[MAX_LENGTH_INPUT_STRING + 2] = '$';	//used later for length error detection
	in_buf_pointer = 0;
	in_buf[0]='\0';
	do read_stdin(in_buf, &in_buf_pointer, MAX_LENGTH_INPUT_STRING + 3, (logged ? current_user : NULL));
	while (in_buf_pointer == 0);	//loop to ignore '\n' as input
	
	if (in_buf[MAX_LENGTH_INPUT_STRING + 2] == '\0') {	//happens if the input was too long
		if (in_buf[MAX_LENGTH_INPUT_STRING + 1] != '\n') //no need to flush input if only 1 exceeding char
			flush_input();
		printf("Command too long. Try again!\n\n");
		return -1;
	}
	ret = sscanf(in_buf, "%" STR(MAX_LENGTH_COMMAND) "s", comm_name);
	cmd_code = get_cmd_code(comm_name);
	ret = get_num_args(cmd_code);	//get the number of the command arguments
	switch(ret) {
		case -1:
			printf("Invalid command. Try again!\n\n");
			return -1;
		case 0:		//Command without arguments
			ret = sscanf(in_buf + strlen(comm_name), "%s", unexpected_arg);	//try to spot excess arguments
			if (ret > 0) {
				printf("Unexpected argument for command %s. Try again!\n\n", comm_name);
				return -1;
			}
			return cl_exec_command(cmd_code, NULL);
		case 1:		//Command with an argument
			//parse the argument (also spots excess ones, if any)
			ret = sscanf(in_buf + strlen(comm_name), "%" STR(MAX_LENGTH_USERNAME) "s %s", user_name, unexpected_arg);
			if (ret != 1) {
				printf("Bad arguments for command %s. Try again!\n\n", comm_name);
				return -1;
			}
			return cl_exec_command(cmd_code, user_name);
	}
	return -1;
}

/* Parses a message from the console.*/
int get_message(char** msg, uint16_t* msg_len) {
	*msg_len = 0;
	*msg = (char*)malloc(MAX_LENGTH_MESSAGE + 1);
	(*msg)[0] = '\0';
	read_msg_stdin(*msg, msg_len, MAX_LENGTH_MESSAGE);
	return 0;
}

/* Reads the arguments of the main program.
 * If the input isn't valid, terminates the program.*/
void parse_arguments(int argc, char* argv[], struct in_addr* ma, uint16_t* mp, struct in_addr* sa, uint16_t* sp) {
	long parsed_value;
	/*Check the number of arguments*/
	if (argc != 5) {
		printf("Incorrect number of arguments. Correct syntax is <program> <local_IP> <local_port> <server_IP> <server_port>\n");
		exit(1);
	}
	/*Parse the local port*/
	parsed_value = strtol(argv[2], NULL, 10);
	if (!parsed_value) {
		printf("Bad argument (local_port).\n");
		exit(1);
	}
	if (parsed_value < 1 || parsed_value > 65535) {
		printf("Local port number must be in range [1, 65535].\n");
		exit(1);
	}
	*mp = (unsigned short)parsed_value;
	/*Parse the server port*/
	parsed_value = strtol(argv[4], NULL, 10);
	if (!parsed_value) {
		printf("Bad argument (server_port).\n");
		exit(1);
	}
	if (parsed_value < 1 || parsed_value > 65535) {
		printf("Server port number must be in range [1, 65535].\n");
		exit(1);
	}
	*sp = (unsigned short)parsed_value;
	/*Parse the local IP*/
	if (!inet_pton(AF_INET, argv[1], ma)) {		//invalid ip
		printf("Invalid local IP address format.\n");
		exit(1);
	}
	/*Parse the server IP*/
	if (!inet_pton(AF_INET, argv[3], sa)) {		//invalid ip
		printf("Invalid IP address format for server.\n");
		exit(1);
	}
}

/* Modifies the configuration of the I/O terminal.
 * Input in raw mode.*/
void init_console_config() {
	#ifdef __linux__
    tcgetattr( STDIN_FILENO, &oldt);	//save old configuration
    newt = oldt;
    newt.c_lflag &= ~(ICANON);			//disable buffered input
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);	//enable the new config
    #endif
}

/* Resets the initial configuration of the I/O terminal.*/
void reset_console_config() {
	#ifdef __linux__
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);	//restore the initial config
    #endif
}
