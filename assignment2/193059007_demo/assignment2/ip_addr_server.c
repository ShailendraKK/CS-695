

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
#include <signal.h>
#include <pthread.h>
#include "open_file.c"
#define MAXEVENTS 1000
static int socket_fd, epoll_fd;
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
	
	char data[10];
	int count;
	
	count = read(fd, data, sizeof(data) - 1);
		if (count == -1) {
			
			printf("Network Error: Could not receive data\n");
			return;
		}
	    data[count] = '\0';
        if(strcmp(data,"get_ip")==0){
           char* resp = read_from_file("server_ip",161);

            	count = write(fd, resp, strlen(resp));
		        if (count == -1) {
			    
			    printf(" Error while writing");
			      return;
				  
		            }
	
        
		
		}
	

		
		
	
	

}




int main(int argc,char *argv[])
{
	int port=8090;

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

	events =  calloc(MAXEVENTS, sizeof(event));

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