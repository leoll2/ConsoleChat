/*This file contains all the network-related functions of the server.*/

#include "server.h"

/* Closes the socket and removes it from the master set.*/
void close_sock_clear_set(int sock) {
	close(sock);
	FD_CLR(sock, &master_fds);
	printf("Closed socket %d\n", sock);
}

/* Gets the command (and the arguments, if any) from the client via TCP.
 * Returns 0 if success, -1 otherwise.*/
int get_cmd_from_cl(int sock, uint16_t* cmd, char** username) {
	int ret, num_args;
	
	ret = recv(sock, (void*)cmd, sizeof(uint16_t), 0);
	CHECK_RECV_RET(sock, "Failed to receive the command");
	*cmd = ntohs(*cmd);
	if (*cmd >= NUM_OF_COMMANDS) {
		printf("Received an invalid command (with code %u) from socket %d.\n", (unsigned int)*cmd, sock);
		return -1;
	}
	num_args = get_num_args(*cmd);
	if (num_args > 0) {
		uint16_t usrlen;
		ret = recv(sock, (void*)&usrlen, sizeof(uint16_t), 0);
		CHECK_RECV_RET(sock, "Error: failed to receive the argument length");
		usrlen = ntohs(usrlen);
		*username = (char*)malloc(usrlen);
		ret = recv(sock, (void*)*username, usrlen, 0);
		CHECK_RECV_RET(sock, "Error: failed to receive the argument");
	}
	return 0;
}

/* Receives a message from a client via TCP.
 * Returns 0 if success, -1 otherwise.*/
int read_message(int sock, char* sender_name, char* dest_name) {
	int ret;
	uint16_t msg_len;
	char* msg_body;
	
	ret = recv(sock, (void*)&msg_len, sizeof(uint16_t), 0);
	CHECK_RECV_RET(sock, "Error: failed to receive the message length");
	msg_len = ntohs(msg_len);
	msg_body = (char*)malloc(msg_len);
	ret = recv(sock, (void*)msg_body, msg_len, 0);
	CHECK_RECV_RET(sock, "Error: failed to receive the message");
	printf("User %s sent an offline message to %s (length = %d).\n", sender_name, dest_name, msg_len);
	return store_message(sender_name, dest_name, msg_body);
}

/* Receives IP and port from the client for instant messages.
 * Returns 0 if success, -1 otherwise.*/
int request_addr_from_cl(int sock, struct in_addr* ip, uint16_t* port) {
	int ret;
	ret = recv(sock, (void*)ip, sizeof(struct in_addr), 0);
	CHECK_RECV_RET(sock, "Failed to get the IP from the host");
	ret = recv(sock, (void*)port, sizeof(uint16_t), 0);
	CHECK_RECV_RET(sock, "Failed to get the port from the host");
	*port = ntohs(*port);
	return 0;
}

/* Sends the recipient's address to another host that wants to send an offline message
 * to the former. The address is sent as plain text (ASCII).
 * Returns 0 if success, -1 otherwise.*/
int send_dest_addr_to_cl(int sock, struct sockaddr_in full_addr) {
	int ret;
	char text_addr[100];		//send 100 bytes, it's always enough to store the address in text format
	sprintf(text_addr, "%hu %hu %u", full_addr.sin_family, full_addr.sin_port, full_addr.sin_addr.s_addr);
	ret = send(sock , (void*)text_addr , 100, 0);
	CHECK_SEND_RET(sock, "Failed to send the recipient address to the client");
	return 0;
}

/* Sends a single offline message to a client that just reconnected.
 * This is a sub-function of send_offline_messages_to_cl().
 * Returns 0 if success, -1 otherwise.*/
int send_offline_msg(int sock, message* msg) {
	int ret;
	uint16_t sender_len, body_len, netsender_len, netbody_len;
	//find the lengths
	sender_len = strlen(msg->sender) + 1;
	body_len = strlen(msg->body_text) + 1;
	netsender_len = htons(sender_len);
	netbody_len = htons(body_len);
	//send the message
	ret = send(sock, (void*)&netsender_len, sizeof(uint16_t), 0);
	CHECK_SEND_RET(sock, "Failed to send the message sender length to the client");
	ret = send(sock, (void*)&netbody_len, sizeof(uint16_t), 0);
	CHECK_SEND_RET(sock, "Failed to send the message body length to the client");
	ret = send(sock, (void*)(msg->sender), sender_len, 0);
	CHECK_SEND_RET(sock, "Failed to send the message sender to the client");
	ret = send(sock, (void*)(msg->body_text), body_len, 0);
	CHECK_SEND_RET(sock, "Failed to send the message body to the client");
	return 0;
}

/* Sends all the buffered offline messages to a client that just reconnected.
 * Returns 0 if all messages were sent, -1 otherwise*/
int send_offline_messages_to_cl(int sock, list_chat_user_node* user_node) {
	int ret;
	uint16_t zero;
	list_message_node* lmnp;
	
	while (((chat_user*)user_node->data)->pending_msg != NULL) {	//if the buffer isn't empty
		lmnp = ((chat_user*)user_node->data)->pending_msg;			//take the first message in the list
		ret = send_offline_msg(sock, (message*)(lmnp->data));		//send it
		RETURN_ON_ERROR;
		lmnp = list_message_extract_head(&(((chat_user*)user_node->data)->pending_msg));
		list_message_node_delete(lmnp);								//and delete it from the buffer
	}
	//notify the client that there are no more messages left (submitting 0 as sender length)
	zero = htons(0);	//not really needed
	ret = send(sock, (void*)&zero, sizeof(uint16_t), 0);
	CHECK_SEND_RET(sock, "Failed to send 'end of message' to the client");
	return 0;
}

/* Sends the response code to any command issued by a host.
 * Returns 0 if success, -1 otherwise.*/
int send_response_to_cl(int sock, uint16_t response_code) {
	int ret;
	uint16_t netresp;
	netresp = htons(response_code);
	ret = send(sock, (void*)&netresp, sizeof(response_code), 0);
	CHECK_SEND_RET(sock, "Failed to send the response code to the client");
	return 0;
}

/* Sends the list of all users to a client that ran the command !who.
 * The list is sent as a unique plain text string.
 * Returns 0 if success, -1 otherwise.*/
int send_users_list_to_cl(int sock) {
	char* buf;
	int ret;
	uint32_t list_len = 1;	//null character
	uint32_t netlist_len;
	list_chat_user_node* p = lcu;
	
	//compute the length of the buffer
	while (p != NULL) {
		int name_len;
		chat_user* cu;
		cu = (chat_user*)p->data;
		name_len = strlen(cu->username);
		list_len += name_len;
		if (cu->online)
			list_len += 9;	//(online)\n  -> 9 characters
		else
			list_len += 10;	//(offline)\n -> 10 characters
		p = p->next;
	}
	netlist_len = htonl(list_len);
	//send the list length to client
	ret = send(sock, (void*)&netlist_len, sizeof(uint32_t), 0);
	CHECK_SEND_RET(sock, "Failed to send the list length to the client");
	//allocate the buffer;
	buf = (char*)malloc(list_len);
	buf[0] = '\0';
	//fill the buffer
	p = lcu;
	while (p != NULL) {
		chat_user* cu;
		cu = (chat_user*)p->data;
		strcat(buf, cu->username);
		if (cu->online)
			strcat(buf, "(online)\n");
		else
			strcat(buf, "(offline)\n");
		p = p->next;
	}
	//send the buffer to client
	ret = send(sock, (void*)buf, list_len, 0);
	CHECK_SEND_RET(sock, "Failed to send the list to the client");
	//free memory
	free(buf);
	return 0;
}