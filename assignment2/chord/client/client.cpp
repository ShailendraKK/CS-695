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
#include "KVClientLibrary.h"
using namespace std;

int sock_fd; 
	char command[7];
	char key[257];
	struct hostent *hostdetails;
	char value[262145];
	char input[262404];
	string key_rev="";
	string value_rev="";
	string message_rev="";
	struct sockaddr_in server_address; 
	FILE *fin,*fout;


  /***select **/
   fd_set set;
   struct timeval timeout;
   

   

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
 	    check=send(sock_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
 	    //check=write(sock_fd,send_char_data,strlen(send_char_data));
		if(check==-1){
			fprintf(fout,"Network Error: Could not send data\n");
			return -1;
		}

		strcpy(resp,"");
		FD_ZERO(&set);
        FD_SET(sock_fd,&set);

        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;
		int rv = select(sock_fd+1,&set,NULL,NULL,&timeout);
		if(rv == -1)
  			  return 0; 
  		else if(rv == 0){
    		fprintf(fout,"Network Error: timeout\n");
			return 0; 
		  }
  		else
    		count=read(sock_fd, resp, sizeof(resp));
	    
		if(count==-1){
	    	fprintf(fout,"Network Error: Could not receive data\n");
	    	return 0;
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
					fprintf(fout,"%s\n",value_rev.c_str());
				}
				else{
					fprintf(fout,"%s,%s\n",key,value_rev.c_str());
				}
			}
		}
		else
		{
			if(message_rev.compare("Success")==0 || message_rev.compare("Does not exist")==0)
			{
				//cout <<message_rev<<endl;
				fprintf(fout,"%s\n",message_rev.c_str());
			}
			else{
				//cout<<"Error : "<<message_rev<<endl;
				fprintf(fout,"Error : %s\n",message_rev.c_str());
			}
		}

	
		
	
	
	}
	else{
		fprintf(fout,"XML Error: Received unparseable message\n");
	}
	
	return 0;
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
	
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    fprintf(fout,"Network Error: Could not create socket\n");
	    exit(1);
	}
    hostdetails = gethostbyname("192.168.122.158");
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(port);  
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    fprintf(fout,"Network Error: Could not connect\n");
	    exit(1);
	}
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

 
   /*****/

   timeout.tv_sec = 0;
   timeout.tv_usec = 1000000;
   

	while(fgets(input,262404,fin)!=NULL){
		if(handle()==-1)
		{
			break;
		}
	}
   close(sock_fd);
   if(fin!=stdin)
   {
   fclose(fin);
	}
   fclose(fout);
	return 0;
}
