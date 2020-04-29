#include<string.h>
#include <stdio.h>
#include<stdlib.h>
#include "open_file.c"

char** get_server_ip_address(char *file_path){
    char **ip_addr = calloc(sizeof(char)*150,sizeof(char));
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

    char ** ip_addr=get_server_ip_address("../../development/server_ip");
    char ** ip_addr2=calloc(sizeof(char)*150,sizeof(char));
    printf("%d",get_total_ip_addr(ip_addr));
    // for(int i=0;i<10;i++){
    //     if(ip_addr[i]!=NULL){
    //      printf("%s\n",ip_addr[i]);   
    //     }
    // }
   // printf("done");



    return 0;
}*/