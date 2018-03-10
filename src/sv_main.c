/* This is the main file of the server.*/

#include "server.h"

list_chat_user lcu;		//list of all registered users
fd_set master_fds;		//main set of file descriptors

int main(int argc, char* argv[]) {
	/*VARIABLES*/
	int fdmax;			//max number of descriptors
	int listener;		//listening socket
	int newfd;			//new socket
	fd_set read_fds;	//read set
	uint16_t sv_port;	//input port
	int ret, i, addrlen;
	struct sockaddr_in sv_addr, cl_addr;	//server and client addresses
	
	/*PARSE ARGUMENTS*/
	sv_port = parse_port(argc, argv);
	
	/*INITIALIZATION*/
	init_handlers();
	lcu = list_chat_user_new();
	FD_ZERO(&master_fds);
	FD_ZERO(&read_fds);
	listener = socket(AF_INET, SOCK_STREAM, 0);
	FAIL_ABORT(listener, "Failed sock()");
	memset(&sv_addr, 0, sizeof(sv_addr));
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_addr.s_addr = INADDR_ANY;
	sv_addr.sin_port = htons(sv_port);
	
	/*BIND*/
	ret = bind(listener, (struct sockaddr*)& sv_addr, sizeof(sv_addr));
	FAIL_ABORT(ret, "Failed bind()");
	/*LISTEN*/
	ret = listen(listener, BACKLOG_SIZE);
	FAIL_ABORT(ret, "Failed listen()");
	printf("Created listening socket. Port = %hu, backlog queue size = %d\n", sv_port, BACKLOG_SIZE);
	
	FD_SET(listener, &master_fds);	//add listener to master set
	fdmax = listener;			//update max
	
	/*RUN THE SERVER*/
	while(1) {
		read_fds = master_fds;
		select(fdmax+1, &read_fds, NULL, NULL, NULL);
		
		for(i=0; i<= fdmax; ++i) {			//inspect the whole set
			if(FD_ISSET(i, &read_fds)) { 	//if any descriptor is ready
                if(i == listener) {
					/*Listener desc. is ready, so there's a pending connection request*/
					addrlen = sizeof(cl_addr);
                    newfd = accept(listener, (struct sockaddr *)&cl_addr, (unsigned int*)&addrlen);
					FAIL_WARN(newfd, "Failed accept()") else {
						FD_SET(newfd, &master_fds);				//add new descriptor to set
						if(newfd > fdmax){ fdmax = newfd; } //update id of max descriptor
						printf("A new host connected to the server, socket = %d.\n", newfd);
					}
				} else {
					/*A host is sending bytes*/
					ret = get_command(i);
					if (ret == -2) {	//only !quit can return -2
						close_sock_clear_set(i);
					}
				}
			}
		}
	}
	close(listener);
	return 0;
}