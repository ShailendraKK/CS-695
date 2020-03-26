#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include "open_file.c"
#define MAX_DOMAINS 5

virConnectPtr conn;
char *vm_name[MAX_DOMAINS];
int vm_index=0;
static void customErrorFunc(void *userdata, virErrorPtr err)
{
  fprintf(stderr, "Failure of libvirt library call:\n");
  fprintf(stderr, " Code: %d\n", err->code);
  fprintf(stderr, " Domain: %d\n", err->domain);
  fprintf(stderr, " Message: %s\n", err->message);
  fprintf(stderr, " Level: %d\n", err->level);
  fprintf(stderr, " str1: %s\n", err->str1);
  fprintf(stderr, " str2: %s\n", err->str2);
  fprintf(stderr, " str3: %s\n", err->str3);
  fprintf(stderr, " int1: %d\n", err->int1);
  fprintf(stderr, " int2: %d\n", err->int2);
}

char* get_ip_addr(virDomainPtr dom,int create_new)
{
virDomainInterfacePtr *interfaces = NULL;
    int interface_count =0;
    char *ip=NULL;
    size_t i;
    if ((interface_count = virDomainInterfaceAddresses(dom, &interfaces,VIR_DOMAIN_INTERFACE_ADDRESSES_SRC_AGENT,0)) < 0)
  {
      printf("Error while getting ip address of guest");
      
      if (interfaces && interface_count > 0)
        for (i = 0; i < interface_count; i++)
            virDomainInterfaceFree(interfaces[i]);
     free(interfaces);
     return NULL;
    }
      for (i = 0; i < interface_count; i++) {
    if(strcmp(interfaces[i]->name,"enp1s0")==0){
         
        virDomainIPAddressPtr ip_addr = interfaces[1]->addrs;
        //printf("ip addr: %s ", ip_addr->addr);
        ip= ip_addr->addr;
    
    }    
}
save_to_file("server_ip",ip,create_new);
return ip;
}

 char **inactive_vm;
char * get_an_inactive_vm(virConnectPtr conn){
            int total_vm = virConnectNumOfDefinedDomains(conn);
            if(total_vm>0){

    inactive_vm = malloc(sizeof(char*)*total_vm);
    total_vm = virConnectListDefinedDomains(conn,inactive_vm,total_vm);
    return inactive_vm[0];
            }
            else{
                return NULL;
            }
    
}
char **active_vm;

int get_active_vm(){
  virDomainPtr *domains;
size_t i;
char *dom_name;
int ret=-1;


  active_vm = malloc(sizeof(char*)*MAX_DOMAINS);

unsigned int flags = VIR_CONNECT_LIST_DOMAINS_RUNNING |
                     VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
ret = virConnectListAllDomains(conn, &domains, flags);
if (ret < 0)
    return ret;
for (i = 0; i < ret; i++) {
  dom_name = (char *) virDomainGetName(domains[i]);
  active_vm[i]= dom_name;
  printf("Guest %s is running\n",active_vm[i]);
  vm_name[vm_index] = active_vm[i];
    vm_index++;
   char *ip = get_ip_addr(domains[i],0);
    if(ip !=NULL){
      printf("ip address: %s\n",ip);
    }
}
free(domains);

return ret;
}



struct sigaction old_action;

void sigint_handler(int sig_no)
{
    fclose(fopen("server_ip", "w"));
     virConnectClose(conn);
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}








int spawn_new_vm=0;
int spawn_vm_index=0;
virDomainPtr create_new_VM(){
  virDomainPtr dom_new;
  char* ip=NULL;
  if(get_an_inactive_vm(conn) == NULL){
        printf("No active VM found ...exiting");
          return NULL;
    }
  
    dom_new=virDomainLookupByName(conn, get_an_inactive_vm(conn));
    

    if (!dom_new) {
        fprintf(stderr, "Unable to load guest configuration");
        return NULL;
    }

    if (virDomainCreate(dom_new) < 0) {
        fprintf(stderr, "Unable to boot guest configuration");
        return NULL;
    }
    printf("Guest %s has booted up\n",inactive_vm[0]);
    vm_name[vm_index] = inactive_vm[0];
    vm_index++;
    sleep(40);
     ip = get_ip_addr(dom_new,0);
    if(ip !=NULL){
      printf("ip address: %s\n",ip);
    }

    return dom_new;

}
double max(double val1,double val2){
    return (val1>=val2?val1:val2);
}
double min(double val1,double val2){
   return (val1<=val2?val1:val2);
}

int vm_running=0;
int destroy_some_vm=0;
virDomainPtr dom_destry=NULL;
int high_count[MAX_DOMAINS]={0};
int low_count[MAX_DOMAINS]={0};
int post_destroy_task(const char *dom_name){
   int num_doms;
    virDomainPtr dom_ptr[MAX_DOMAINS];
     virDomainStatsRecordPtr *retStats;
     int first_time=0;
  if((num_doms=virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING))!=-1){
     for(int i=0;i<num_doms;i++){
       
     dom_ptr[i]=retStats[i]->dom;
     if(strcmp(dom_name,virDomainGetName(dom_ptr[i]))!=0){
       
     if(first_time==0) {
        first_time=1;
       get_ip_addr(dom_ptr[i],1);
     }
      else {
        get_ip_addr(dom_ptr[i],0);
      }
       }
         low_count[i]=0;
     }
     
  }
  return 1;

}
void * get_cpu_utilization(){
  virDomainStatsRecordPtr *retStats;
  virTypedParameter params;
  int num_doms=0;
  
  //int vm_destroy=0;
  int64_t diff;
  virDomainPtr dom_ptr[MAX_DOMAINS];

  int i,j;
  while(1)
  {
  if((num_doms=virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING))!=-1){
      
      vm_running=num_doms;
      int64_t pre_cpu_time[num_doms],cpu_time[num_doms];
      double cpu_util[num_doms];
      
      for(i=0;i<num_doms;i++){
           dom_ptr[i]=retStats[i]->dom;
       for(j=0;j<(retStats[i]->nparams);j++){
         params=retStats[i]->params[j];
         if(strcmp(params.field,"cpu.time")==0){
            pre_cpu_time[i] = params.value.l;;
         }

      }
   
  }
 
  sleep(3);

    virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING);
    
      for(i=0;i<num_doms;i++){
       for(j=0;j<retStats[i]->nparams;j++){
         
         params=retStats[i]->params[j];
         if(strcmp(params.field,"cpu.time")==0){
            cpu_time[i] = params.value.l;
         }

       }
         /* if(refresh_ip==1){
         if(i==0) get_ip_addr(dom_ptr[i],1);
         else get_ip_addr(dom_ptr[i],0);
         low_count[i]=0;
      }
      if(i==num_doms-1)
      {
      refresh_ip=0;
      vm_destroy=0;
      }*/
      diff= (cpu_time[i]-pre_cpu_time[i])/3;
      cpu_util[i] = ((double)(100 * diff))/1000000000;
      printf("\n %s -> CPU utilization : %lf \n",virDomainGetName(dom_ptr[i]),max(0.0,min(100.0,cpu_util[i])));
          if(cpu_util[i]>49){
            
                if(high_count[i]>=2)
                {
                  high_count[i]=0;
                  low_count[i]=0;
                  if(spawn_vm_index < MAX_DOMAINS){
                       if(spawn_new_vm==0) spawn_new_vm=1;
                   }
                   else{
                     printf("Max VM limit reached");
                   }
                 }
                 if(spawn_new_vm ==0)
                 high_count[i]++;
              }
             else if (cpu_util[i]<21){
              
               
               if(low_count[i]>=4 && virConnectNumOfDomains(conn)>1 && spawn_new_vm==0){
                     
                     low_count[i]=0;
                     high_count[i]=0;
                    const char * dom_name = virDomainGetName(dom_ptr[i]);
                    post_destroy_task(dom_name);
                      sleep(5);
                     virDomainDestroy(dom_ptr[i]);
                    
                     printf("%s shutting down due to low load\n",dom_name);
                     
                     printf("%s shut down\n",dom_name);
                    
                     
                     
                     
               }
               if(spawn_new_vm ==0){
                low_count[i]++;
                //printf("low_count[%d] incremented: %d",i,low_count[i]);
               }


             }
             else{

               high_count[i]=0;
               low_count[i]=0;

             }
      }

      
  
     
 
                 
     


  }
  else{
    printf("Error while getting cpu stats\n");
  }
}
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


int main(int argc, char *argv[])
{
    

    virSetErrorFunc(NULL, customErrorFunc);
virDomainPtr dom;
char *ip=NULL;
pthread_t pid_t;
  struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
    
    conn = virConnectOpen("qemu:///system");
    if (conn == NULL) {
        fprintf(stderr, "Failed to open connection to qemu:///system\n");
        return 1;
    }
    if(get_active_vm()>0){
      pthread_create(&pid_t,NULL,&get_cpu_utilization,NULL);
    }
    else{
    if(get_an_inactive_vm(conn) == NULL){
        printf("No active VM found ...exiting");
            virConnectClose(conn);
        return -1;
    }
  
    dom=virDomainLookupByName(conn, get_an_inactive_vm(conn));
    

    if (!dom) {
        fprintf(stderr, "Unable to load guest configuration");
        return -1;
    }

    if (virDomainCreate(dom) < 0) {
        fprintf(stderr, "Unable to boot guest configuration");
        return -1;
    }
    printf("Guest %s has booted up\n",inactive_vm[0]);
   
    vm_name[0]=inactive_vm[0];
    vm_index++;
    free(inactive_vm); 
    
    sleep(50);
    ip = get_ip_addr(dom,0);
    if(ip !=NULL){
      printf("ip address: %s\n",ip);
    }
    pthread_create(&pid_t,NULL,&get_cpu_utilization,NULL);
    } 
    
    
   while(1){
   
        if(spawn_new_vm==1){
         

        create_new_VM();
         spawn_new_vm =0;
     
       
      }
      // if(destroy_some_vm==1 && dom_destry!=NULL){
      //        destroy_some_vm=0;
      //        get_active_vm();
      //       virDomainDestroy(dom_destry);
      //       printf("%s shutting down due to low load",virDomainGetName(dom_destry));
            
      //       sleep(10);
      // }
       
      sleep(1);
    }
    

    pthread_join(pid_t, NULL);
    
    virDomainFree(dom);
   
    

   
    
      
   
     
   virConnectClose(conn);

    return 0;
}