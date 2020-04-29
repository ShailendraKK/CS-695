#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
int main(){
    char *mem;
    int i=0;
    while(1){
        if((mem = malloc((10<<20)))==NULL){
            printf("Memory limity reached");
            return 0;
        }
        memset(mem,0,(10<<20));
        i=i+10;
        printf("%d MB\n",i);
        sleep(1);
    }
    return 0;
}