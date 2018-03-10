#include <stdlib.h>
#include <stdio.h>
#include "struct_util.h"


/*Message*/
message* new_message(char* sender, char* content, bool instant) {
	message* nm;
	nm = (message*)malloc(sizeof(message));
	nm->body_text = (char*)malloc(strlen(content) + 1);
    nm->sender = (char*)malloc(MAX_LENGTH_USERNAME + 1);
	nm->instant = instant;
	strcpy(nm->sender, sender);
	strcpy(nm->body_text, content);
	return nm;
}

bool delete_message(message* m) {
    if (!m)
        return false;
    if (m->sender)
        free(m->sender);
    if (m->body_text)
        free(m->body_text);
    free(m);
    return true;
}


/*Sockaddr_in*/
struct sockaddr_in build_sockaddr_in(char* ip, unsigned short port) {
	struct sockaddr_in nsi;
	nsi.sin_family = AF_INET;
	nsi.sin_port = htons(port);
	inet_pton(AF_INET, ip, &nsi.sin_addr);
	return nsi;
}

struct sockaddr_in build_sockaddr_in2(struct in_addr ip, unsigned short port) {
	struct sockaddr_in nsi;
	nsi.sin_family = AF_INET;
	nsi.sin_port = htons(port);
	nsi.sin_addr = ip;
	return nsi;
}

/*Chat_user*/
bool delete_chat_user(chat_user* cu) {
    if (!cu)
        return false;
    if (cu->username)
        free(cu->username);
    if (cu->pending_msg)
        list_message_remove_all(&(cu->pending_msg));
    free(cu);
    return true;
}

chat_user* new_chat_user(char *name, int sockfd, struct sockaddr_in full_addr) {
	chat_user* ncu;
	if (strlen(name) > MAX_LENGTH_USERNAME) {
		printf("Attempted to create a too long username\n");
		return NULL;
	}
	ncu = (chat_user *)malloc(sizeof(chat_user));
    ncu->username = (char *)malloc(MAX_LENGTH_USERNAME + 1);
	strcpy(ncu->username, name);
	ncu->full_address = full_addr;
	ncu->pending_msg = NULL;
    ncu->online = true;
    ncu->sock = sockfd;
	return ncu;
}

chat_user* new_chat_user2(char *name, int sockfd, struct in_addr ip, unsigned short port) {
    return new_chat_user(name, sockfd, build_sockaddr_in2(ip, port));
}
    
chat_user* new_chat_user3(char *name, int sockfd, char* ip, unsigned short port) {
	return new_chat_user(name, sockfd, build_sockaddr_in(ip, port));
}
