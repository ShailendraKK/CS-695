
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include "ThreadPool.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <signal.h>
#include <pthread.h>
#include <condition_variable>
#include "KVCache.hpp"
#define MAXEVENTS 1000
using namespace std;
static int socket_fd, epoll_fd;
int set_num=6;
string key,value,command;
pthread_mutex_t* lock_main;
pthread_cond_t* cv;
int* cv_arr;
int thread_pool_size=8;
struct sigaction old_action;	
auto thread_pool=new ThreadPool();
static int non_blocking_socket(int socket_fd)
{
	int flags;

	flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags == -1) {
		printf("Unknown Error: Error while getting flags from socket file descriptor\n");
		return -1;
	}

	flags |= O_NONBLOCK;
	if (fcntl(socket_fd, F_SETFL, flags) == -1) {
		printf("Unknown Error: Error while setting O_NONBLOCK flag in socket file descriptor\n");
		return -1;
	}

	return 0;
}

void read_data(int fd)
{
	
	char xml[262404];
	int count;
	
	while ((count = read(fd, xml, sizeof(xml) - 1))) {
		if (count == -1) {
			if (errno == EAGAIN)
				return;
			printf("Network Error: Could not receive data\n");
			break;
		}
		xml[count] = '\0';
		string input_job(xml);
		
		thread_pool->add_work_to_job_pool(fd,input_job);
		
	}
	
	printf("Closed connection on descriptor: %d\n", fd);
	close(fd);
}


void sigint_handler(int sig_no)
{
    create_xml();
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}




int main(int argc,char *argv[])
{
	int port=8080;
	string input_string;
	int set_size=2;
	if(argc>1){
		char *token;
		if(argc < 5){
			printf("Usage: ./KVServer -port=XXX -threadPoolSize=YYY -numSetsInCache=ZZZ -sizeOfSet=AAA");
			return 0;
		}
		else{
			for(int i=1;i<5;i++){
				input_string=argv[i];
				char input[input_string.length()+1];
				strcpy(input,input_string.c_str());


				token=strtok(input,"=");
				token=strtok(NULL,"=");
				/*while(token!=NULL){
					token=strtok(NULL,"=");
				}*/
				switch(i){
					case 1: port=atoi(token);
							break;
					case 2: thread_pool_size=atoi(token);
							break;
					case 3: set_num=atoi(token);
							break;
					case 4: set_size=atoi(token);
							break;
				}
			}
		}
	}
	pthread_mutex_t lock1[set_num];

	init(set_size,set_num);
//lock_main = lock1;
	pthread_cond_t cv1[set_num];

//	cv=cv1;
	int cv_arr1[set_num];
	cv_arr=cv_arr1;
for(int i=0;i<set_num;i++){
	pthread_mutex_init(&lock1[i],NULL);
	pthread_cond_init( &cv1[i], NULL);
}
lock_main = lock1;
	cv=cv1;
	  struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);

	struct epoll_event event, *events;
	struct sockaddr_in server_address;
	int flag = 1;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
          printf("Network Error: Could not create socket\n");
          exit(1);
    }
    if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int)) == -1) {
            printf("Unknwon Error: Error while settig socket flag\n");
            exit(1);
    }       
    server_address.sin_family = AF_INET;        
    server_address.sin_port = htons(port);    
    server_address.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_address.sin_zero),8);
      if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address))== -1) {

            printf("Unknown Error: Error while binding\n");
            exit(1);
    }

	if (non_blocking_socket(socket_fd) == -1)
		exit(1);
       
    if (listen(socket_fd, 1000) == -1) {
         printf("Unknown Error: Error while Listening on port\n");
         exit(1);
    }
     fflush(stdout);

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		printf("Unknown Error: Error while epoll setup\n");
		exit(1);
	}

	event.data.fd = socket_fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
		printf("Unknown Error: Error while perfomrming operation on epoll file descriptor");
		exit(1);
	}

	events = (epoll_event *) calloc(MAXEVENTS, sizeof(event));

	while(1) {
		int n, i;
		n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++) {
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP ||
			    !(events[i].events & EPOLLIN)) {
				perror("Unknown Error: No Socket ready\n");
				close(events[i].data.fd);
			} else if (events[i].data.fd == socket_fd) {
				struct epoll_event event;
				struct sockaddr address;
				socklen_t soc_len = sizeof(address);
				int client_fd;
				char hostip[NI_MAXHOST], serverport[NI_MAXSERV];
				while ((client_fd = accept(socket_fd, &address, &soc_len)) != -1) {
					if (getnameinfo(&address, soc_len,hostip, sizeof(hostip),serverport, sizeof(serverport),NI_NUMERICHOST | NI_NUMERICHOST) == 0) {
						printf("Accepted connection on descriptor %d \n",client_fd);
					}
					if (non_blocking_socket(client_fd) == -1) {
						abort();
					}
					event.data.fd = client_fd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
						printf("Unknown Error:Error while performing operation on epoll_ctl\n");
						abort();
					}
					soc_len = sizeof(address);
				}

				if (errno != EAGAIN && errno != EWOULDBLOCK)
					printf("Unknown Error: Error while accepting connection\n");


			} else {
					read_data(events[i].data.fd);
			}
		}
	}

	free(events);
	close(socket_fd);
	return 0;
}