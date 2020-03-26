#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include<signal.h>
#include "open_file.c"

#include <sys/time.h>
#include "KVClientLibrary.h"
using namespace std;
int ip_fd=0;
char *server_ip[10];
int sock_fd[10]={0}; 
	char command[7];
	char key[257];
	int temp_index=0;
	struct hostent *hostdetails;
	char value[262145];
	int index_ip=0;
	char input[262404];
	string key_rev="";
	string value_rev="";
	string message_rev="";
	struct sockaddr_in server_address; 
	FILE *fin,*fout;
	
int connect_to_socket(char* ip,int port){
    int sock_fd_temp; 
    struct hostent *hostdetails;
	
    struct sockaddr_in server_address; 
    if ((sock_fd_temp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    fprintf(fout,"Network Error: Could not create socket\n");
	    exit(1);
	}
    hostdetails = gethostbyname(ip);
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(port);  
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd_temp, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    fprintf(fout,"Network Error: Could not connect\n");
	     return -1;
	}
	return sock_fd_temp;
}

struct sigaction old_action;
void sigint_handler(int sig_no)
{
    fclose(fopen("server_ip", "w"));
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}



int handle(){
		int check;
		//string xml=convert_to_xml(input);
		if(input[0]!='\n')
		{
		strcpy(key,"");
		strcpy(value,"");
		strcpy(command,"");
		
		char* token=strtok(input,",");

		strcpy(command,token);
		for(int i=0;i<strlen(command); i++)
       {
         if(command[i]>='A' && command[i]<='Z')
           {
            command[i] = command[i] + 32;
           }
        }

		int count=0;
		char resp[1000];
		while(token!=NULL)
		{ 
        	token = strtok(NULL,",");
        	count++;
        	if(count==1){
        		if(token!=NULL){

        		strcpy(key,token);
        		}
        		else
        		{
        			fprintf(fout,"XML Error: Received unparseable message\n");
        			return 0;
        		}
        	}
        	else{
        		if(token!=NULL)
        		{
        			if(strcmp(command,"get")==0 || strcmp(command,"del")==0){
        				fprintf(fout,"XML Error: Received unparseable message\n");
        				return 0;
        		   }
        		  strcat(value,token);
        		 strcat(value,",");	
        		}
        		else{
        			int len=strlen(value);
        			value[len-2]='\0';
        		} 
        	}

		}
		if(strcmp(command,"put")==0 && strcmp(value,"")==0){
			fprintf(fout,"XML Error: Received unparseable message\n");
			return 0;
		}
		int key_len=strlen(key);
		int value_len=strlen(value);
		if(key[key_len-1]=='\n')
		{
			key[key_len-1]='\0';
		}
		if(key[key_len-2]=='\r')
		{
			key[key_len-2]='\0';
		}
		if(value[value_len-1]=='\n')
		{
			value[value_len-1]='\0';
		}
		if(value[value_len-2]=='\r')
		{
			value[value_len-2]='\0';
		}
		string xml=convert_to_xml(command,key,value);
		
	    char send_char_data[xml.length()+1]; 
 	    strcpy(send_char_data,xml.c_str());
		  temp_index = temp_index%index_ip;
 	    check=send(sock_fd[temp_index],send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
		
 	    //check=write(sock_fd,send_char_data,strlen(send_char_data));
		if(check==-1){
			fprintf(fout,"Network Error: Could not send data\n");
			return -1;
		}

		strcpy(resp,"");
		count=read(sock_fd[temp_index], resp, sizeof(resp));
	    if(count==-1){
	    	fprintf(fout,"Network Error: Could not receive data\n");
	    	return -1;
	    }
		
	    char xml_c[count+1];

		for(int i=0;i<count;i++){
			xml_c[i]=resp[i];
		}

		resp[count]='\0';
		xml_c[count]='\0';
		parse_xml(message_rev,key_rev,value_rev,xml_c);

		if(strcmp(command,"get")==0){
			if(strcmp(key,key_rev.c_str())==0)
			{
			/*cout<<key<<",";
			cout<<value_rev<<endl;*/
				if(value_rev.compare("Does not exist")==0)
				{
					fprintf(fout,"%s : %s\n",server_ip[temp_index],value_rev.c_str());
				}
				else{
					fprintf(fout,"%s : %s,%s\n",server_ip[temp_index],key,value_rev.c_str());
				}
			}
		}
		else
		{
			if(message_rev.compare("Success")==0 || message_rev.compare("Does not exist")==0)
			{
				//cout <<message_rev<<endl;
				fprintf(fout,"%s : %s\n",server_ip[temp_index],message_rev.c_str());
			}
			else{
				//cout<<"Error : "<<message_rev<<endl;
				fprintf(fout,"%s : Error : %s\n",server_ip[temp_index],message_rev.c_str());
			}
		}
		 temp_index++;

	
		
	
	
	}
	else{
		fprintf(fout,"XML Error: Received unparseable message\n");
	}
	
	return 0;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}


 char ** ip_addr=get_server_ip_address("server_ip");
    char ** ip_addr2=get_server_ip_address("server_ip");
void *  initialize_ip_addr(void * arg){
	while(1){
	char* send_char_data="get_ip";
	int check=send(ip_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
		//check=write(sock_fd,send_char_data,strlen(send_char_data));
		if(check==-1){
			fprintf(fout,"Network Error: Could not send data\n");
			
		}

		char resp[161];
		int count=read(ip_fd, resp, sizeof(resp)-1);
	    if(count==-1){
	    	fprintf(fout,"Network Error: Could not receive data\n");
	    	
	    }
		resp[count]='\0';
		save_to_file("server_ip",resp,1);



		ip_addr= get_server_ip_address("server_ip");
		
	
		if(check_new_ip_added(ip_addr,ip_addr2)==1){
			ip_addr2[index_ip]=get_new_server_ip(ip_addr,ip_addr2);
			server_ip[index_ip]=ip_addr2[index_ip];
			sock_fd[index_ip] =connect_to_socket(server_ip[index_ip],8080);
			index_ip++;
		}	
		msleep(500);	
	}
}
void remove_ip(int index){
	printf("Disconnecting from %s\n",server_ip[index]);
     for(int i=index;i<index_ip-1;i++){
		 server_ip[i]=server_ip[i+1];
		 ip_addr2[i]=ip_addr2[i+1];
		 close(sock_fd[i]);
		 sock_fd[i]=sock_fd[i+1];

	 }
	 index_ip--;

}

int main(int argc, char *argv[])
{


	int port=8080;
	
	if(argc==3){
				char * infilename=argv[1];
		fin=fopen (infilename, "r");
		char* outfilename=argv[2];
		fout=fopen(outfilename,"w");
	}
	else if(argc==4){
		char * infilename=argv[1];
		fin=fopen (infilename, "r");
		char* outfilename=argv[2];
		fout=fopen(outfilename,"w");
		port=atoi(argv[3]);
	}
	else{
			fin=stdin;
		fout=stdout;
		
	}
	  struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
	/*
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    fprintf(fout,"Network Error: Could not create socket\n");
	    exit(1);
	}
    hostdetails = gethostbyname("127.0.0.1");
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(port);  
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    fprintf(fout,"Network Error: Could not connect\n");
	    exit(1);
	}*/
	/*if(argc==3){

		while(fgets(input,262404,fin)!=NULL){
		if(handle()==-1)
		{
			break;
		}
		}

	}
   else
   {
	while(fgets(input,262404,stdin)!=NULL){
		if(handle()==-1)
		{
			break;
		}
	}
	
   }*/
   ip_fd =connect_to_socket("localhost",8090);
   
  /*
   int total_ip=get_total_ip_addr(ip_addr);
   if(total_ip==0){
	   fprintf(fout,"No server ip address found");
   }
   int i;
   for(i=0;i<total_ip;i++){
	   sock_fd[i]=connect_to_socket(ip_addr[i],8080);
	   server_ip[i]=ip_addr[i];
   }
   index_ip = i;
	*/
int ip_rem_index=0;
	while(1){
	fseek(fin, 0, SEEK_SET);
	while(fgets(input,262404,fin)!=NULL){

		char* send_char_data="get_ip";
	int check=send(ip_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
		//check=write(sock_fd,send_char_data,strlen(send_char_data));
		if(check==-1){
			fprintf(fout,"Network Error: Could not send data\n");
			
		}

		char resp[161];
		int count=read(ip_fd, resp, sizeof(resp)-1);
	    if(count==-1){
	    	fprintf(fout,"Network Error: Could not receive data\n");
	    	
	    }
		resp[count]='\0';
		save_to_file("server_ip",resp,1);



		ip_addr= get_server_ip_address("server_ip");
		if((ip_rem_index=check_ip_removed(ip_addr,ip_addr2))!=-1){
			remove_ip(ip_rem_index);
		}
		if(check_new_ip_added(ip_addr,ip_addr2)==1){
			ip_addr2[index_ip]=get_new_server_ip(ip_addr,ip_addr2);
			server_ip[index_ip]=ip_addr2[index_ip];
			printf("Connecting to %s\n",server_ip[index_ip]);
			sock_fd[index_ip] =connect_to_socket(server_ip[index_ip],8080);
			index_ip++;
		}

		if(handle()==-1)
		{
			break;
		}

       //msleep(10);
	}

}/*
pthread_join(pid_t, NULL);
for(i=0;i<total_ip;i++){
   close(sock_fd[i]);
}*/
   if(fin!=stdin)
   {
   fclose(fin);
	}
   fclose(fout);
	return 0;
}