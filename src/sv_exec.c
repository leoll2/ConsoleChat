/* This file contains all the functions needed to perform the execution
 * of commands and their accessory operations (server side).*/

#include "server.h"

/* Creates a message object and puts it into the
 * recipient's mailbox.
 * Returns 0 if success, -1 otherwise.*/
int store_message(char* sender, char* dest, char* body) {
	message* m;
	list_chat_user_node* user_node;
	list_message* lmp;
	user_node = list_chat_user_find_by_name(lcu, dest);
	if (!user_node)
		return -1;
	lmp = &(((chat_user*)(user_node->data))->pending_msg);
	m = new_message(sender, body, false);
	list_message_add(lmp, m);
	printf("Stored offline message (%s -> %s)\n", sender, dest);
	return 0;
}

/* Note: !help is handled by the client alone, needs no help
 * from the server. This unused function is kept here just
 * for possible future upgrades.*/
int sv_exec_help(int sock) {
	return 0;
}

/* Handles the !register command.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_register(int sock, char* username) {
	int ret;
	uint16_t port;
	struct in_addr ip;
	list_chat_user_node* user_node;
	user_node = list_chat_user_find_by_name(lcu, username);
	if (user_node != NULL && ((chat_user*)user_node->data)->online == true) {		//if user already exists and is online
		printf("User %s attempted to login twice.\n", username);
		ret = send_response_to_cl(sock, REG_DOUBLE_LOGIN);
		RETURN_ON_ERROR;
		return 0;
	} else if (user_node != NULL && ((chat_user*)user_node->data)->online == false) {	//if user already exists and isn't online
		((chat_user*)user_node->data)->online = true;
		((chat_user*)user_node->data)->sock = sock;
		printf("User %s reconnected.\n", username);
		ret = send_response_to_cl(sock, REG_RECONNECTED);
		RETURN_ON_ERROR;
		ret = request_addr_from_cl(sock, &ip, &port);
		RETURN_ON_ERROR;
		((chat_user*)user_node->data)->full_address.sin_port = htons(port);
		((chat_user*)user_node->data)->full_address.sin_addr = ip;
		ret = send_offline_messages_to_cl(sock, user_node);
		if (ret < 0) {
			printf("Failed to send offline messages to the host.\n");
			return -1;
		}
		printf("Sent offline messages to %s after reconnection\n", username);
		return 0;
	} else {	//if the user is new
		chat_user* ncu;
		ret = send_response_to_cl(sock, REG_NEW_USER);
		RETURN_ON_ERROR;
		ret = request_addr_from_cl(sock, &ip, &port);
		RETURN_ON_ERROR;
		ncu = new_chat_user2(username, sock, ip, port);
		list_chat_user_add_head(&lcu, ncu);
		printf("Registered new user %s\n", username);
		return 0;
	}	
}

/* Handles the !deregister command.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_deregister(int sock) {
	int ret;
	char* username = NULL;
	list_chat_user_node* usernode = list_chat_user_find_by_socket(lcu, sock);
	if (usernode)
		username = ((chat_user*)(usernode->data))->username;
	if (!username) {
		printf("Failed to deregister user with socket %d, not found!\n", sock);
		ret = send_response_to_cl(sock, DEREG_FAILED);
		return -1;
	}
	printf("Deregistered user %s.\n", username);
	list_chat_user_remove(&lcu, username);
	ret = send_response_to_cl(sock, DEREG_DONE);
	RETURN_ON_ERROR;
	return 0;
}

/* Handles the !who command.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_who(int sock) {
	int ret;
	if (!lcu) {
		ret = send_response_to_cl(sock, WHO_EMPTY);
		RETURN_ON_ERROR;
		return 0;
	}
	ret = send_response_to_cl(sock, WHO_READY);
	RETURN_ON_ERROR;
	ret = send_users_list_to_cl(sock);
	RETURN_ON_ERROR;
	return 0;
}

/* Handles the !send command.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_send(int sock, char* dest_name) {
	int ret;
	list_chat_user_node* p;
	struct sockaddr_in full_addr;
	char* sender_name = NULL;
	
	list_chat_user_node* sendernode = list_chat_user_find_by_socket(lcu, sock);
	if (sendernode)
		sender_name = ((chat_user*)(sendernode->data))->username;
		
	p = list_chat_user_find_by_name(lcu, dest_name);
	if (p!=NULL && ((chat_user*)(p->data))->online == true) {	//if recipient is online
		ret = send_response_to_cl(sock, SEND_ONLINE_USER);
		RETURN_ON_ERROR;
		full_addr = ((chat_user*)(p->data))->full_address;
		ret = send_dest_addr_to_cl(sock, full_addr);
		RETURN_ON_ERROR;
	} else if (p!=NULL && ((chat_user*)(p->data))->online == false) {	//if recipient is offline
		uint16_t cl_response;
		ret = send_response_to_cl(sock, SEND_OFFLINE_USER);
		RETURN_ON_ERROR;
		ret = recv(sock, (void*)&cl_response, sizeof(uint16_t), 0);
		CHECK_RECV_RET(sock, "Error: failed to get a response from the host");
		cl_response = ntohs(cl_response);
		switch(cl_response) {
			case SEND_NO_OFFLINE:
				printf("User with socket %d refused to send an offline message to %s.\n", sock, dest_name);
				return 0;
			case SEND_OK_OFFLINE:
				printf("User with socket %d accepted to send an offline message to %s.\n", sock, dest_name);
				return read_message(sock, sender_name, dest_name);
			default:
				printf("Error: unknown response from client. Refuse assumed.\n");
				return -1;
		}
	} else {	//if recipient doesn't exist (p == NULL)
		ret = send_response_to_cl(sock, SEND_NONEXIST_USER);
		RETURN_ON_ERROR;
	}
	return 0;
}

/* Handles the !quit command.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_quit(int sock) {
	list_chat_user_node* p;
	p = list_chat_user_find_by_socket(lcu, sock);
	if (!p)
		printf("A non-registered host (sock = %d) disconnected\n", sock);
	else {
		((chat_user*)(p->data))->online = false;
		((chat_user*)(p->data))->sock = -1;
		printf("User %s disconnected\n", ((chat_user*)(p->data))->username);
	}
	return -2;
}

/* Redirects any command to the appropriate handler.
 * Returns 0 if success, -1 otherwise.*/
int sv_exec_command(int sock, uint16_t cmd_code, char* username) {
	int ret;
	ret = exec_comm(sock, cmd_code, username);
	return ret;
}

/* Initialize the command handlers.*/
void init_handlers() {
	set_handler(CMD_HELP, TO_GF(sv_exec_help));
	set_handler(CMD_REGISTER, TO_GF(sv_exec_register));
	set_handler(CMD_DEREGISTER, TO_GF(sv_exec_deregister));
	set_handler(CMD_WHO, TO_GF(sv_exec_who));
	set_handler(CMD_SEND, TO_GF(sv_exec_send));
	set_handler(CMD_QUIT, TO_GF(sv_exec_quit));
}
