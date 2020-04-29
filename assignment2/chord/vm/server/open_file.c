#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

char* read_from_file(char * file_name,int size){
    int fd = open(file_name,O_RDONLY);
    int count=0;
    char *file_content=(char *)malloc(sizeof(char)*size);
    
    if((count=read(fd,file_content,sizeof(char)*size))==-1){
        printf("Error while reading file");
    }
    close(fd);
    file_content[count]='\0';

    return file_content;
}
