#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define showError(msg) do { perror(msg); exit(EXIT_FAILURE); \
                            } while(0)
#define STACK_SIZE (1024*1024)


static int change_root_using_pivot(const char *new_root,const char *put_old){
    return syscall(SYS_pivot_root,new_root,put_old);
}

int write_to_file(char *content,char *file_path){
    int fd = open(file_path,O_WRONLY);
    if(fd==-1)
        return -1;
    int count = write(fd,content,strlen(content));
    if(count ==-1)
        return -1;
    close(fd);
}

int set_cgroup_mem_limit(int pid,char *mem_limit){
    char buf[255];
    sprintf(buf,"/sys/fs/cgroup/memory/%d",pid);
    mkdir(buf,0555);
    char buf1[255];
    strcpy(buf1,buf);
    strcat(buf1,"/memory.limit_in_bytes");

    if(write_to_file(mem_limit,buf1)==-1)
        return -1;
    char swap[] = "0";
    strcpy(buf1,buf);
    strcat(buf1,"/memory.swappiness");
    if(write_to_file(swap,buf1)==-1)
        return -1;
    char pid_c[255];
    sprintf(pid_c,"%d",pid);
    strcpy(buf1,buf);
    strcat(buf1,"/tasks");
    if(write_to_file(pid_c,buf1)==-1)
        return -1;
    return 0;

}

void setip(char *name,char *ip_addr,char * netmask){
  struct ifreq ifr;
  struct sockaddr_in *addr;
  int fd = socket(PF_INET,SOCK_DGRAM,IPPROTO_IP);
  ifr.ifr_addr.sa_family = AF_INET;
  
  memcpy(ifr.ifr_name,name,IF_NAMESIZE-1);
  addr = (struct sockaddr_in * ) &ifr.ifr_addr;
  inet_pton(AF_INET,ip_addr,&addr->sin_addr);
  ioctl(fd,SIOCSIFADDR,&ifr);
  inet_pton(AF_INET,netmask,&addr->sin_addr);
  ioctl(fd,SIOCSIFNETMASK,&ifr);

  ioctl(fd,SIOCGIFFLAGS,&ifr);
  memcpy(ifr.ifr_name,name,IF_NAMESIZE-1);
  
  ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
  ioctl(fd,SIOCSIFFLAGS,&ifr);
  close(fd);
}

void network_name_space(char *vethname,char *ip){
    setip(vethname,ip,"255.0.0.0");
}

void uts_name_space(char * hostname){
    struct utsname uts;

    if(sethostname(hostname,strlen(hostname))==-1)
        showError("uts_name_sapce : sethostname");

    // if(uname(&uts)==-1)
    //     showError("uts_name_space: uname");

    // printf("uts_name_space : hostname in child : %s",uts.nodename);

}

void pid_name_space(){
    printf("pid_name_space : child PID  = %ld\n", (long) getpid());
    printf("pid_name_space : parent PPID = %ld\n", (long) getppid());
    mkdir("/proc",0555);
    if(mount("proc","/proc","proc",0,NULL)==-1)
        showError("pid_name_space : mount");
    printf("pid_name_space :procfs mounted at %s \n","/proc");
    
}

void mount_name_space(char *arg){
    char *new_root = arg;
    const char *put_old = "/oldrootfs";
    char path[PATH_MAX];
    
    /***
     * Checking whether mount event are propagating to parent or not. FYI they should not
     */

    if(mount(NULL,"/",NULL,MS_REC | MS_PRIVATE,NULL)==1)
        showError("mount -> MS_PRIVATE");

    /**
     * Checking whether new_root is actually a mout point or not
     */
    if(mount(new_root,new_root,NULL,MS_BIND,NULL)==-1)
        showError("mount -> MS_BIND");
    
    /***
     * Creating directory to which old root will be pivoted
     */

    snprintf(path,sizeof(path),"%s/%s",new_root,put_old);
    if(mkdir(path,0777)==-1)
        showError("mkdir");

    if(change_root_using_pivot(new_root,path)==-1)
        showError("pivot_root");

    if(chdir("/")==-1)
        showError("chdir");

    if(umount2(put_old,MNT_DETACH)==-1)
        showError("umount2");
    if(rmdir(put_old)==-1)
        perror("rmdir");
}

static int child_func(void *arg){
    char **args = (char**)arg;
    printf("Container : Creating mount namespace\n");
    mount_name_space(args[1]);
    printf("Container : Creating pid namespace\n");
    pid_name_space();
    printf("Container : Creating uts namespace\n");
    uts_name_space(args[2]);
    printf("Container : Creating network namespace\n");
    network_name_space(args[4],args[6]);
    execlp("/bin/sh", "/bin/sh", (char *) NULL);
    showError("execlp");  /* Only reached if execlp() fails */

}

int main(int argc,char *argv[]){

    char *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if(stack == MAP_FAILED)
        showError("mmap");
    
    pid_t pid = clone(child_func,stack + STACK_SIZE,
                CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | SIGCHLD , argv);
    
    if(pid == -1)
        showError("clone");
    printf("Container : child pid = %d \n",pid);
    if(set_cgroup_mem_limit(pid,argv[7])==-1){
        showError("Container: memory limit");
    }
    else{
        printf("Container : Cgroup memory limit set %s Bytes\n",argv[7]);
    }
        
    char buf[255];
    sprintf(buf,"ip link add name %s type veth peer name %s netns %d",argv[3],argv[4],pid);
    system(buf);
    setip(argv[3],argv[5],"255.0.0.0");

    if(wait(NULL)==-1)
        showError("wait");

    exit(EXIT_SUCCESS);
}