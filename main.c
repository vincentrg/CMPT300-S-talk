#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "list.h"
#include "main.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int s;
struct sockaddr_in addr, remote;
//char *condition;
int control = 0;

void *key_in( void *ptr ){	
	char user_in[256];
	char *flag = "!";

	LIST *l1 = (LIST*) ptr;

	while(control == 0){	
		fgets(user_in, 256, stdin);
		user_in[strlen(user_in) - 1] = '\0'; //remove new line character

		if(*user_in == '!'){
			control = 1;

			shutdown(s, SHUT_RDWR);
		}else{
			pthread_mutex_lock(&mutex1);

			ListAdd(l1, user_in);

			pthread_mutex_unlock(&mutex1);
		}

	}
	pthread_exit(0);
}

void *send_out( void *ptr ){ 
	LIST *l1 = (LIST*) ptr;
	char *msg;
	char *removed;

	while(control == 0){
		if(l1->curitem != NULL){
			pthread_mutex_lock(&mutex1);

			msg = ListFirst(l1);
			removed = ListRemove(l1);

			pthread_mutex_unlock(&mutex1);

			sendto(s, msg, 256, 0, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));
		}
	}

	pthread_exit(0);
}

void *print_screen( void *ptr ){
	LIST *l2 = (LIST*) ptr;
	char *msg;
	char *removed;

	while(control == 0){
		if(l2->curitem != NULL){
			pthread_mutex_lock(&mutex2);

			msg = ListFirst(l2);
			removed = ListRemove(l2);

			pthread_mutex_unlock(&mutex2);

			printf("partner: %s\n", msg);
		}
	}
	pthread_exit(0);
}

void *recv_in( void *ptr ){
	LIST *l2 = (LIST*) ptr;
	char msg[256];
	socklen_t fromlen = sizeof(remote);

	while(control == 0){
		recvfrom(s, msg, 256, 0, (struct sockaddr *)&remote, &fromlen);

		pthread_mutex_lock(&mutex2);

		ListAdd(l2, msg);

		pthread_mutex_unlock(&mutex2);
	}
	pthread_exit(0);
}

int main (int argc, char *argv[]){
	int rem_port, home_port, t1, t2, t3, t4;

	struct hostent *he;

	pthread_t input, send, print, recieve;

	//create socket

	s = socket(AF_INET, SOCK_DGRAM, 0);

	//extract port numbers from input
	home_port = atoi(argv[1]);
	rem_port = atoi(argv[3]);
	
	//addr initilization
	addr.sin_family = AF_INET;
	addr.sin_port = htons(home_port); 
	addr.sin_addr.s_addr = INADDR_ANY; 

	//remote initilization
	remote.sin_family = AF_INET;
	remote.sin_port = htons(rem_port);  

	he = gethostbyname(argv[2]);

	if (he == NULL){
		printf("host unavailable \n");
	}

	//grab IP info and convert to s_addr
	inet_aton(he->h_addr, &remote.sin_addr);

	//bind socket
	bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

	//create lists
	LIST *l1 = ListCreate();
	LIST *l2 = ListCreate();

	//create threads
	t1 = pthread_create(&input, NULL, key_in, l1);
	t2 = pthread_create(&send, NULL, send_out, l1);
	t3 = pthread_create(&print, NULL, print_screen, l2);
	t4 = pthread_create(&recieve, NULL, recv_in, l2);
	
	//join threads
	pthread_join(input, NULL);
	pthread_join(send, NULL);
	pthread_join(print, NULL);
	pthread_join(recieve, NULL);

	//close socket
	close(s);

	return 0;
}
