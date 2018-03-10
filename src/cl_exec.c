/* This file contains all the functions needed to perform the execution
 * of commands and their accessory operations (client side).*/

#include "client.h"

/* Handles the !help command.
 * Returns 0*/
int cl_exec_help() {
	show_commands();
	return 0;
}

/* Handles the !register command.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_register(char* username) {
	int ret;
	uint16_t response_code;
	if (logged) {
		printf("Error: can't register when there's already another user logged in this client.\n\n");
		return -1;
	}
	ret = send_cmd_to_sv(CMD_REGISTER, username);
	RETURN_ON_ERROR;
	ret = recv_response_from_sv(&response_code);
	RETURN_ON_ERROR;
	else if (response_code == REG_DOUBLE_LOGIN) {		//if the user is already online
		printf("Failed to register, the user %s is already online!\n\n", username);
		ret = -1;
	} else if (response_code == REG_NEW_USER) {			//if the user is new
		ret = send_addr_to_sv();
		FAIL_ERROR2(ret, "Failed to send the address to the server.\n");
		printf("The registration was successful. Welcome %s!\n\n", username);
		ret = 0;
	} else if (response_code == REG_RECONNECTED) {
		ret = send_addr_to_sv();
		FAIL_ERROR2(ret, "Failed to send the address to the server.\n");
		printf("Welcome back %s!\n", username);
		ret = download_offline_msg();
		FAIL_ERROR2(ret, "Failed to download offline messages.\n");
		ret = 0;
	} else {
		printf("Unknown error occurred during registration.\n\n");
		ret = -1;
	}
	if (ret == 0) {		//if no error occurred, login the user
		current_user = (char*)malloc(strlen(username) + 1);
		strcpy(current_user, username);
		logged = true;
	}
	return ret;
}

/* Handles the !deregister command.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_deregister() {
	int ret;
	uint16_t response_code;
	if (!logged) {
		printf("Error: you must register to see the list of active users!\n\n");
		return -1;
	}
	ret = send_cmd_to_sv(CMD_DEREGISTER, NULL);
	RETURN_ON_ERROR;
	ret = recv_response_from_sv(&response_code);
	RETURN_ON_ERROR;
	switch(response_code) {
		case DEREG_FAILED:
			printf("Error: failed to deregister the user!\n\n");
			return -1;
		case DEREG_DONE:
			printf("Successfully deregistered!\n\n");
			logged = false;
			free(current_user);
			return 0;
	}
	return -1;
}

/* Handles the !who command.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_who() {
	int ret;
	uint16_t response_code;
	if (!ENABLE_UNREG_WHO && !logged) {
		printf("You must register to see the list of active users!\n\n");
		return -1;
	}
	ret = send_cmd_to_sv(CMD_WHO, NULL);
	RETURN_ON_ERROR;
	ret = recv_response_from_sv(&response_code);
	RETURN_ON_ERROR;
	switch (response_code) {
		case WHO_EMPTY:
			printf("Nobody is online right now\n\n");
			break;
		case WHO_READY:
			ret = recv_users_list_from_sv();
			FAIL_ERROR2(ret, "Failed to receive the list of the users.\n");
			break;
		default:
			printf("Unknown response from server\n\n");
			return -1;
	}
	return 0;
}

/* Handles the !send command.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_send(char* dest_username) {
	int ret;
	uint16_t msg_len;
	char* msg = NULL;
	uint16_t response_code;
	uint16_t chatmode;
	struct sockaddr_in dest_addr;
	
	if (!logged) {
		printf("You must register to send a message!\n\n");
		return -1;
	}
	ret = send_cmd_to_sv(CMD_SEND, dest_username);
	RETURN_ON_ERROR;
	ret = recv_response_from_sv(&response_code);
	RETURN_ON_ERROR;
	switch (response_code) {
		case SEND_NONEXIST_USER:
			printf("User %s doesn't exist!\n\n", dest_username);
			return -1;
		case SEND_ONLINE_USER:
			printf("User %s is online!\n", dest_username);
			ret = recv_dest_addr_from_sv(&dest_addr);
			RETURN_ON_ERROR;
			printf("Write your message below!\n");
			ret = get_message(&msg, &msg_len);
			FAIL_ERROR2(ret, "Message aborted, nothing was sent.\n");
			ret = send_udp_msg(msg, dest_addr);
			FAIL_WARN2(ret, "Failed to deliver the message.\n")
			else printf("Instant message sent!\n\n");
			free(msg);
			return ret;
		case SEND_OFFLINE_USER:
			chatmode = ask_to_send_offline(dest_username);
			ret = reply_to_sv(chatmode);
			RETURN_ON_ERROR;
			switch (chatmode) {
				case SEND_NO_OFFLINE:
					printf("\nMessage aborted, nothing was sent!\n\n");
					return 0;
				case SEND_OK_OFFLINE:
					printf("\nWrite your message below!\n");
					ret = get_message(&msg, &msg_len);
					FAIL_ERROR2(ret, "Message aborted, nothing was sent!\n");
					ret = send_msg_to_sv(msg, msg_len);
					if (ret < 0) {
						printf("Message aborted, nothing was sent!\n\n");
						free(msg);
						return -1;
					}
					free(msg);
					return 0;
				default:
					return -1;
			}
		default:
			printf("Error: unknown response from server.\n\n");
			return -1;
	}
	return 0;
}

/* Handles the !quit command.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_quit() {
	int ret;
	ret = send_cmd_to_sv(CMD_QUIT, NULL);
	FAIL_WARN2(ret, "Failed to explicitly warn the server about disconnection.\n");
	return -2;	//returning -2 terminates the main program
}

/* Redirects any command to the appropriate handler.
 * Returns 0 if success, -1 otherwise.*/
int cl_exec_command(uint16_t cmd_code, char* username) {
	int ret;
	pthread_mutex_lock(&console_mutex);
	ret = exec_comm(SOCK_UNNECESSARY, cmd_code, username);	//no need to specify socket, it's a global variable
	pthread_mutex_unlock(&console_mutex);
	return ret;
}

/* Initialize the command handlers*/
void init_handlers() {
	set_handler(CMD_HELP, TO_GF(cl_exec_help));
	set_handler(CMD_REGISTER, TO_GF(cl_exec_register));
	set_handler(CMD_DEREGISTER, TO_GF(cl_exec_deregister));
	set_handler(CMD_WHO, TO_GF(cl_exec_who));
	set_handler(CMD_SEND, TO_GF(cl_exec_send));
	set_handler(CMD_QUIT, TO_GF(cl_exec_quit));
}
