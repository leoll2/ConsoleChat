/*This file contains all the network-related functions of the client.*/

#include "client.h"

/* Downloads from the server all the messages received while offline.
 * Returns 0 if success, -1 otherwise.*/
int download_offline_msg() {
	int ret;
	uint16_t sender_len, body_len;
	char *sender, *body;
	bool told_to_host = false;		//used to notify the host about the presence of offline msg
	
	while(1) {
		//receive the length of the sender name
		ret = recv(sd, (void*)&sender_len, sizeof(uint16_t), 0);
		FAIL_ERROR(ret, "Error while receiving a message");
		sender_len = ntohs(sender_len);
		
		if (sender_len == 0)	//if there isn't any message left (coded as 0 in the sender length field)
			break;
		if (!told_to_host) {	//a header to display before the first message gets printed
			printf("Here are the messages you received while offline!\n");
			told_to_host = true;
		}
		//receive the length of the message body
		ret = recv(sd, (void*)&body_len, sizeof(uint16_t), 0);
		FAIL_ERROR(ret, "Error while receiving a message");
		body_len = ntohs(body_len);
		//allocate memory
		sender = (void*)malloc(sender_len);
		body = (void*)malloc(body_len);
		//receive the sender name
		ret = recv(sd, (void*)sender, sender_len, 0);
		if (ret < 0) {
			perror("Error while receiving a message");
			free(sender);
			free(body);
			return -1;
		}
		//receive the body of the message
		ret = recv(sd, (void*)body, body_len, 0);
		if (ret < 0) {
			perror("Error while receiving a message");
			free(sender);
			free(body);
			return -1;
		}
		//print the message
		printf("%s (offline msg)>\n%s\n", sender, body);
		//free memory
		free(sender);
		free(body);
	}
	if (!told_to_host)	//if the mailbox was empty
		printf("You didn't receive any message while offline.\n");
	return 0;
}

/* Retrieves IP and port of the recipient from the server when
 * attempting to send an instant message.
 * Returns 0 if success, -1 otherwise.*/
int recv_dest_addr_from_sv(struct sockaddr_in* dest_addr) {
	char text_addr[100];		//server sends 100 bytes, not all are needed though
	int ret;
	ret = recv(sd, (void*)text_addr, 100, 0);
	FAIL_ERROR(ret, "Failed to get the destination address from the server");
	sscanf(text_addr, "%hu %hu %u", &dest_addr->sin_family, &dest_addr->sin_port, &dest_addr->sin_addr.s_addr);
	return 0;
}

/* Receive the response of the server after sending a command to it.
 * Returns 0 if success, -1 otherwise.*/
int recv_response_from_sv(uint16_t* response_code) {
	int ret;
	ret = recv(sd, (void*)response_code, sizeof(uint16_t), 0);
	FAIL_ERROR(ret, "Failed to get the response from the server");
	*response_code = ntohs(*response_code);
	return 0;
}

/* Attempts to receive an instant UDP message. If any, stores it in the
 * display buffer and returns 0. If no message is available or an error
 * occurs, returns -1.*/
int recv_udp_msg(char** msgbuf, char* recvbuf, int* next) {
	uint16_t namelen, bodylen;
	uint32_t addrlen;
	int ret;
	char format[25];
	char *name, *body;
	struct sockaddr_in connecting_addr;
	ret = recvfrom(udp_sock, recvbuf, MAX_LENGTH_MESSAGE + MAX_LENGTH_USERNAME + 14, 0,
					(struct sockaddr*)&connecting_addr, &addrlen);
	RETURN_ON_ERROR;
	//Store
	sscanf(recvbuf, "%hu;%hu;%*s", &namelen, &bodylen);
	*msgbuf = (char*)malloc(namelen + bodylen + strlen(" (instant msg)>\n") + 1);
	sprintf(format, "%%hu;%%hu;%%%huc%%%huc", namelen, bodylen);
	name = (char*)malloc(namelen + 1);
	body = (char*)malloc(bodylen + 1);
	sscanf(recvbuf, format, &namelen, &bodylen, name, body);
	name[namelen]='\0';
    body[bodylen]='\0';
	sprintf(*msgbuf, "%s (instant msg)>\n%s", name, body);
	++(*next);
	free(name);
	free(body);
	return ret;
}

/* Gets the list of the registered users from the server.
 * The list is a plain text string, ready to be printed.
 * Returns 0 if success, -1 otherwise.*/
int recv_users_list_from_sv() {
	uint32_t list_len, netlist_len;
	char* buf;
	int ret;
	ret = recv(sd, (void*)&netlist_len, sizeof(uint32_t), 0);
	FAIL_ERROR(ret, "Failed to receive the length of the list");
	list_len = ntohl(netlist_len);
	buf = (char*)malloc(list_len);
	ret = recv(sd, (void*)buf, list_len, 0);
	FAIL_ERROR(ret, "Failed to receive the list");
	printf("Registered clients:\n%s\n", buf);
	free(buf);
	return 0;
}

/* Sends a response to the server, specifically to say whether the
 * client accepted to send a message to an offline client.
 * Returns 0 if success, -1 otherwise.*/
int reply_to_sv(uint16_t chatmode) {
	chatmode = htons(chatmode);
	int ret = send(sd, (void*)&chatmode, sizeof(uint16_t), 0);
	FAIL_ERROR(ret, "Failed to reply to the server");
	return 0;
}

/* Sends IP and port to the server.*/
int send_addr_to_sv() {
	int ret;
	uint16_t netmp;
	netmp = htons(my_port);
	ret = send(sd, (void*)&my_ip, sizeof(struct in_addr), 0);
	FAIL_ERROR(ret, "Failed to send the local IP to the server");
	ret = send(sd, (void*)&netmp, sizeof(uint16_t), 0);
	FAIL_ERROR(ret, "Failed to send the local port to the server");
	return 0;
}

/* Send the command (code) and arguments (if any) to the server via TCP.
 * Returns 0 if success, -1 otherwise.*/
int send_cmd_to_sv(uint16_t cmd_code, char* username) {
	int ret;
	uint16_t usrlen, netusrlen, netcmd_code;
	char* buf;
	netcmd_code = htons(cmd_code);
	ret = send(sd, (void*)&netcmd_code, sizeof(uint16_t), 0);
	FAIL_ERROR(ret, "Error while sending the command code");
	if (get_num_args(cmd_code) > 0) {
		usrlen = strlen(username);
		netusrlen = htons(usrlen + 1);
		buf = (char*)malloc(usrlen + 1);
		strcpy(buf, username);
		ret = send(sd, (void*)&netusrlen, sizeof(uint16_t), 0);
		FAIL_ERROR(ret, "Error while sending the argument length");
		ret = send(sd, (void*)buf, usrlen + 1, 0);
		FAIL_ERROR(ret, "Error while sending the command argument");
		free(buf);
	}
	return 0;
}

/* Sends a message to the server.
 * Returns 0 if success, -1 otherwise.*/
int send_msg_to_sv(char* msg, uint16_t msg_len) {
	int ret;
	uint16_t netmsg_len;
	netmsg_len = htons(msg_len);
	ret = send(sd, (void*)&netmsg_len, sizeof(uint16_t), 0);
	FAIL_ERROR(ret, "Failed to send the message length");
	ret = send(sd, (void*)msg, msg_len, 0);
	FAIL_ERROR(ret, "Failed to send the message");
	return 0;
}

/* Sends an instant message to another client.
 * Since UDP is unreliable and likely to scramble the order of packets, it's
 * not recommended to split the message information into multiple packets 
 * without an adequate (and complex) error detection protocol. It's definitely
 * easier to send the whole message as a single chunk: all or nothing.
 * This approach was used here, the message has the format:
 * sender length | body length | sender | body 
 * Everything is plain text (ASCII), numbers too. ';' is used as separator.
 * Returns 0 if success, -1 on error.*/
int send_udp_msg(char* body, struct sockaddr_in dest_addr) {
	int ret;
	uint16_t namelen, bodylen, sendbuflen;
	char sendbuf[MAX_LENGTH_MESSAGE + MAX_LENGTH_USERNAME + 14]; //name_len + body_len + name + body + splitting characters;
	
	namelen = strlen(current_user);
	bodylen = strlen(body);
	sprintf(sendbuf, "%hu;%hu;%s%s", namelen, bodylen, current_user, body);
    sendbuflen = strlen(sendbuf);
	ret = sendto(udp_sock, sendbuf, sendbuflen + 1, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    ret = (ret < 0) ? -1 : 0;
	return ret;
}

/* Show the connection parameters at launch.*/
void show_connection(struct in_addr sa, int sp, int mp) {
	char ipbuf[15];
	inet_ntop(AF_INET, &sa, ipbuf, 15);
	printf("Successfully connected to server %s (port %d).\n"
			"Receiving instant messages on port %d\n\n",
			ipbuf, sp, mp);
}
