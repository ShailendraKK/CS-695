#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

char* read_from_file(char * file_name,int size){
    int fd = open(file_name,O_RDONLY);
    int count=0;
    char *file_content=(char *)malloc(sizeof(char)*size);
    
    if((count=read(fd,file_content,sizeof(char)*size)-1)==-1){
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
char** get_server_ip_address(char *file_path){
    char **ip_addr = (char **)calloc(sizeof(char)*150,sizeof(char));
    int i=0;
    char *tmp;
    char *filecontent = read_from_file(file_path,161);
    
    while((tmp =__strtok_r(filecontent,"\n",&filecontent))){
       
        ip_addr[i]=tmp;
        i++;

        
    }
    return ip_addr;

}
int check_new_ip_added(char ** ip1,char** ip2){
    if(ip1[0]==NULL){
        return -1;
    }
    for(int i=0;ip1[i]!=NULL;i++){
        if(ip2[i]==NULL){
            return 1;
        }
        if(strcmp(ip1[i],ip2[i])!=0){
            return 1;
        }
        
    }
    return 0;

}

char* get_new_server_ip(char **ip1,char**ip2){
    for(int i=0;ip1[i]!=NULL;i++){
        if(ip2[i]==NULL){
            return ip1[i];
        }
        // if(strcmp(ip1[i],ip2[i])!=0){
        //     return 1;
        // }
        
    }
    return NULL;
}

int get_total_ip_addr(char **ip1){
    int i;
    for(i=0;ip1[i]!=NULL;i++){
        
    }
    return i;
}

/*
int main(){
    char *data =read_from_file("config.xml",2000);
    printf("%s",data);
    return 0;
}*/