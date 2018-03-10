/* This is the main file of the client*/

#include "client.h"

int sd;					//socket for tcp communication with the server
int udp_sock;			//socket for udp instant messages
uint16_t my_port;		//port where to receive instant messages
struct in_addr my_ip;	//ip where to receive instant messages
bool logged = false;	//true if the client has registered
char* current_user = NULL;	//nickname of current user (if logged)
pthread_mutex_t console_mutex;	//mutex to prevent output interferences between threads

/* This is the code of the thread which periodically checks for
 * the presence of incoming instant messages. When a message is
 * detected, it is temporarily buffered and displayed as soon as
 * possible, that is when the user isn't busy doing something else
 * (i.e. writing a message). A mutex is used for this purpose.*/
void* recv_inst_msg_routine(void* arg) {
	char* msgbuf[IN_MSG_BUFFER_SIZE];		//array to buffer messages ready to be displayed
	char recvbuf[MAX_LENGTH_MESSAGE + MAX_LENGTH_USERNAME + 14]; //name_len + body_len + name + body + splitting characters
	int next = 0;
	int ret;
	
	sleep(1);		//small initial delay to avoid interfering with welcome messages
	while(1) {
		do {		//collect all incoming messages
			if (next >= IN_MSG_BUFFER_SIZE)	//unless the buffer is already full
				break;
			ret = recv_udp_msg(&msgbuf[next], recvbuf, &next);
		} while (ret > 0);
		if (pthread_mutex_trylock(&console_mutex) == 0) {	//display the messages, if possible
			//print the buffered messages and flush the array
			for (int i = 0; i < next; ++i) {
				adv_printf(msgbuf[i]);
				free(msgbuf[i]);
			}
			next = 0;	//reset the buffer
			pthread_mutex_unlock(&console_mutex);
		}
		sleep(POLLING_TIME);	//wait a bit to repeat the previous job
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	pthread_t recv_routine_thr;	//background thread to receive instant messages
	int ret;
	uint16_t sv_port;			//server port
	struct in_addr sv_ip;		//server IP
	struct sockaddr_in sv_addr, my_addr;
	
	/*Initialize the handlers of commands*/
	init_handlers();
	/*Slightly modify the console for more control on I/O operations*/
	init_console_config();
	
	/*Read the arguments*/
	parse_arguments(argc, argv, &my_ip, &my_port, &sv_ip, &sv_port); //read the arguments
	
	/*Creation of the socket to send and receive instant messages*/
	udp_sock = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, 0);
	FAIL_ABORT(udp_sock, "UDP socket creation failed");
	memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr = my_ip;
    ret = bind(udp_sock, (struct sockaddr*)&my_addr, sizeof(my_addr));
	FAIL_ABORT(ret, "Bind failed");
	/*Creation of the socket to communicate with the server*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	FAIL_ABORT(sd, "TCP socket creation failed");
	memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(sv_port);
    sv_addr.sin_addr = sv_ip;
	/*Connection*/
	ret = connect(sd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    FAIL_ABORT(ret, "Failed connect");
	/*Start receiving instant messages*/
	pthread_mutex_init(&console_mutex, NULL);
	ret = pthread_create(&recv_routine_thr, NULL, recv_inst_msg_routine, NULL);
	FAIL_ABORT(ret, "Failed to initialize the routine to receive instant message");
	/*Print welcome messages*/
	show_connection(sv_ip, sv_port, my_port);
	show_commands();
	
	while(1) {
		int ret;
		if (logged)		//print the line cursor
			printf("%s> ", current_user);
		else printf("> ");
		ret = get_command();	//read and execute a command
		if (ret == -2)	//only !quit can return -2
			break;
	}
	/*Closing routine*/
	close(udp_sock);
	close(sd);
	printf("Disconnected from server.\n");	
	reset_console_config();
	return 0;
}
