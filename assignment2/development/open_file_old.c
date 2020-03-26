 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include <stdlib.h> 
#include <unistd.h>

char* read_from_file(char * file_name,int size){
    int fd = open(file_name,O_RDONLY);
    int count=0;
    char *file_content=malloc(sizeof(char)*size);
    
    if((count=read(fd,file_content,sizeof(char)*size))==-1){
        printf("Error while reading file");
    }
    close(fd);
    file_content[count]='\0';

    return file_content;
}
int save_to_file(char * file_name,char *data,int create_new){
    int fd=-1;
    int temp;
    if (create_new){
        fd = open(file_name,O_CREAT| O_RDWR,S_IRWXU);
    }
    else{
        fd = open(file_name,O_RDWR | O_APPEND);
    }
   
     temp=write(fd,data,strlen(data));
     write(fd,"\n",1);
     close(fd);
     return temp;
     
}
/*
int main(){
    char *data =read_from_file("config.xml",2000);
    printf("%s",data);
    return 0;
}*/