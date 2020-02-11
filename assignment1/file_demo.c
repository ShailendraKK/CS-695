  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
        #include <unistd.h>
        #include<stdio.h>

int main(){
int open_fd = open("demo_two",O_RDWR | O_CREAT | O_APPEND,S_IRWXU);
	
	

	char tmp[]="This is write demo";
    	char read_buff[100]={};
        
	int bytes_written = write(open_fd,tmp,sizeof(tmp)-1);
    	read(open_fd,read_buff,sizeof(read_buff));
    printf("%s",read_buff);



}