#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <gtk/gtk.h>
#include "open_file.c"
#define MAX_DOMAINS 5

enum
{
  VM_NAME = 0,
  STATUS,
  IP_ADDRESS,
  CPU_UTILZATION,
  NUM_COLS
};

  GtkTreeModel        *model;
  GtkWidget           *view;

struct vm{
    char * server_name;
    char * status;
    char * ip;
    double cpu;
}vm_detail[5];

virConnectPtr conn;
char *vm_name[MAX_DOMAINS];
int vm_index=0;
  GtkWidget *window;
  int display_msg=0;
  char msg_content[40];
double cpu_stats[MAX_DOMAINS];
 GtkWidget *label[MAX_DOMAINS][4];
 GtkWidget *btn_start[MAX_DOMAINS];
 GtkWidget *btn_stop[MAX_DOMAINS];

// char *active_vm[5];
// int active_vm_num=0;
int spawn_new_vm=0;
int spawn_vm_index=0;
 char **inactive_vm;
 int first_time_gui_update=0;
 virDomainPtr *dom_ptr_global;
 extern int max_cpu,min_cpu,vm_wait;
 int num_total_vm_global=0;
 char * vm_to_start=NULL;
 int high_count[MAX_DOMAINS]={0};
int low_count[MAX_DOMAINS]={0};
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



static int
quick_message (gpointer msg)
{
 
 GtkWidget *dialog, *label, *content_area;
 GtkDialogFlags flags;
  gchar* message = msg;
 // Create the widgets
 flags = GTK_DIALOG_DESTROY_WITH_PARENT;
 dialog = gtk_dialog_new_with_buttons ("Message",
                                       GTK_WINDOW(window),
                                       flags,
                                       GTK_STOCK_OK,
                                       GTK_RESPONSE_NONE,
                                       NULL);
 content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
 label = gtk_label_new (message);

 // Ensure that the dialog box is destroyed when the user responds

 g_signal_connect_swapped (dialog,
                           "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);

 // Add the label, and show everything we’ve added

 gtk_container_add (GTK_CONTAINER (content_area), label);
 gtk_widget_show_all (dialog);
  
  return 0;
}

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
virDomainPtr create_new_VM(char* name){
  virDomainPtr dom_new;
  char* ip=NULL;
  if(get_an_inactive_vm(conn) == NULL){
        printf("No active VM found ...exiting");
          spawn_new_vm =0;
          return NULL;
    }
  
   if(name==NULL){
    dom_new=virDomainLookupByName(conn, get_an_inactive_vm(conn));
    
   }
   else{
      dom_new=virDomainLookupByName(conn, name);
   }
    

    if (!dom_new) {
        fprintf(stderr, "Unable to load guest configuration");
        return NULL;
    }

    if (virDomainCreate(dom_new) < 0) {
        fprintf(stderr, "Unable to boot guest configuration");
        return NULL;
    }
    if(name==NULL){
    printf("Guest %s is booting up\n",inactive_vm[0]);
    //   char msg[40]="Guest ";
    //                         strcat(msg,inactive_vm[0]);
    //                         strcat(msg," is booting up");
    //  display_msg=1;                  
    // gdk_threads_add_idle(quick_message,msg);
    
    vm_name[vm_index] = inactive_vm[0];
    
    }
    else{
       printf("Guest %s is booting up\n",name);
       vm_name[vm_index] = name;
      //   char msg[40]="Guest ";
      //                      strcat(msg,name);
      //                      strcat(msg," is booting up");
      // strcpy(msg_content,msg);  
      // display_msg=1;                 
    // gdk_threads_add_idle(quick_message,msg);
    }
    
                          
   // active_vm[active_vm_num++]=inactive_vm[0];
  
    vm_index++;
   
    //  ip = get_ip_addr(dom_new,0,1);

    // if(ip !=NULL){
       
    //   printf("ip address: %s\n",ip);
    // }

    return dom_new;

}

char* get_ip_addr(virDomainPtr dom,int create_new,int save)
{

  if(spawn_new_vm==1) return NULL;
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
if (save==1) 
{ 
  save_to_file("server_ip",ip,create_new);
}
return ip;
}


int get_active_vm(){
  virDomainPtr *domains;
size_t i;
char *dom_name;
int ret=-1;


  //active_vm = malloc(sizeof(char*)*MAX_DOMAINS);

unsigned int flags = VIR_CONNECT_LIST_DOMAINS_RUNNING |
                     VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
ret = virConnectListAllDomains(conn, &domains, flags);
//active_vm_num=ret;
if (ret < 0)
    return ret;
for (i = 0; i < ret; i++) {
  dom_name = (char *) virDomainGetName(domains[i]);
  // active_vm[i]= dom_name;
  printf("Guest %s is running\n",dom_name);
  
  vm_name[vm_index] = dom_name;
    vm_index++;
  //  char *ip = get_ip_addr(domains[i],0,1);
  //   if(ip !=NULL){
      
  //     printf("ip address: %s\n",ip);
  //   }
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

static void
start_resp_vm (GtkWidget *widget,
             gpointer   data)
{
  for(int i=0;i<num_total_vm_global;i++){
    if(widget==btn_start[i]){
                        if(spawn_vm_index < MAX_DOMAINS){
                       if(spawn_new_vm==0) {
                         
                         spawn_new_vm=1;
                         vm_to_start=vm_detail[i].server_name;
                       }

                   }
                   else{
                     printf("Max VM limit reached");
                   }
    }
  }
  for(int i=0;i<MAX_DOMAINS;i++){
                               low_count[i]=0;
                        }
  //g_print ("Hello World\n");
}



static void
stop_resp_vm (GtkWidget *widget,
             gpointer   data)
{
  for(int i=0;i<num_total_vm_global;i++){
    if(widget==btn_stop[i]){
                   
               if(virConnectNumOfDomains(conn)>1 && spawn_new_vm==0){
                    
                          
               
                  
                  
                      
                     if(virDomainDestroy(virDomainLookupByName(conn,vm_detail[i].server_name))==-1){

                       printf("VM error found");
                  
                     }
                    
                        char *vm_name_temp=vm_detail[i].server_name;         
                     printf("%s shut down\n",vm_name_temp);
                     char msg[40]="Shut down ";
                    
                      strcat(msg,vm_name_temp);
                     display_msg=1;
                     strcpy(msg_content,msg); 
                   //gdk_threads_add_idle(quick_message,msg);
                    
    }
  }
  //g_print ("Hello World\n");
}
for(int i=0;i<MAX_DOMAINS;i++){
                               low_count[i]=0;
                        }
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

/*
int post_destroy_task(const char *dom_name,int vm_error){

  if(spawn_new_vm==1)
     return 1;
   int num_doms;
    virDomainPtr dom_ptr[MAX_DOMAINS];
     virDomainStatsRecordPtr *retStats;
     int first_time=0;
     int cond=0;
  if((num_doms=virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING))!=-1){
     for(int i=0;i<num_doms;i++){
      
     dom_ptr[i]=retStats[i]->dom;
     if(dom_name==NULL){
         cond=1;
     }
     else{
        const char *vm_name_temp = virDomainGetName(dom_ptr[i]);
       
       cond = strcmp(dom_name,vm_name_temp);
       
     }
     if(vm_error || (cond!=0)){
       
     if(first_time==0) {
        first_time=1;
       get_ip_addr(dom_ptr[i],1,1);
     }
      else {
        get_ip_addr(dom_ptr[i],0,1);
      }
       }
         
         low_count[i]=0;
     }
     
  }
  for(int temp_i=0;temp_i<active_vm_num;temp_i++){
      if(strcmp(dom_name,active_vm[temp_i])==0){
          active_vm[temp_i]=NULL;
       }
  }
  return 1;

}
*/


void update_gui(){
   GtkListStore  *store;
  GtkTreeIter    iter;
  virTypedParameter params;
   virDomainPtr *dom_ptr;
  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING,G_TYPE_DOUBLE);
virDomainStatsRecordPtr *retStats;
int temp=0;
int first_time=0;

int ret=virConnectListAllDomains(conn, &dom_ptr, VIR_CONNECT_LIST_DOMAINS_PERSISTENT);
   

   for(int i=0;i<ret;i++){
  
 
    char * vm_name_temp= (char *) virDomainGetName(dom_ptr[i]);
     vm_detail[i].server_name = vm_name_temp;
     int state,reason=0;
     virDomainGetState(dom_ptr[i],&state,&reason,0);
     switch (state)
     {
     case 1: 
              if(spawn_new_vm==0)
              {
                if(first_time==0)
                {
                  first_time=1;
                  vm_detail[i].ip = get_ip_addr(dom_ptr[i],1,1);
                }
                else{
                    vm_detail[i].ip = get_ip_addr(dom_ptr[i],0,1);
                }
              }
            vm_detail[i].status="Active";
            vm_detail[i].cpu=cpu_stats[temp++];
     break;
     
     default: 
         vm_detail[i].ip = NULL;
            vm_detail[i].status="InActive";
            vm_detail[i].cpu=0;
       break;
     }
  
         
          GtkLabel *label_tmp;
          label_tmp=GTK_LABEL(label[i][0]);
          gtk_label_set_text(label_tmp,vm_detail[i].server_name);
           label_tmp=GTK_LABEL(label[i][1]);        
          gtk_label_set_text(label_tmp,vm_detail[i].ip);
           label_tmp=GTK_LABEL(label[i][2]);
          gtk_label_set_text(label_tmp,vm_detail[i].status);

          char cpu_utilization[sizeof(vm_detail[i].cpu)];
           sprintf(cpu_utilization,"%lf",vm_detail[i].cpu);
           label_tmp=GTK_LABEL(label[i][3]);
          gtk_label_set_text(label_tmp,cpu_utilization);
          
        
                      
   }


  // model = GTK_TREE_MODEL (store);

  // gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
 
  // g_object_unref (model);
}
void * get_cpu_utilization(){
  virDomainStatsRecordPtr *retStats;
  virTypedParameter params;
  int num_doms=0;
  
  //int vm_destroy=0;
  int64_t diff;
  virDomainPtr dom_ptr[MAX_DOMAINS];

  int i,j;
  int dom_num=0;
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
//   printf("Getting previous values\n");
  sleep(3);

    //virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING);
    
      for(i=0;i<(dom_num=virConnectGetAllDomainStats(conn,VIR_DOMAIN_STATS_CPU_TOTAL,&retStats,VIR_CONNECT_GET_ALL_DOMAINS_STATS_RUNNING));i++){
        if(dom_num!=num_doms){
          for(int i=0;i<MAX_DOMAINS;i++){
            low_count[i]=0;
          }
          //post_destroy_task(NULL,1);
          break;
        }
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
     // printf("Getting current values \n");
      const char *domname=virDomainGetName(dom_ptr[i]);
      if(domname==NULL)
      {
         printf("VM error detected\n");
         for(int i=0;i<MAX_DOMAINS;i++){
            low_count[i]=0;
          }
         // post_destroy_task(NULL,1);
          break;
      }
      diff= (cpu_time[i]-pre_cpu_time[i])/3;
      cpu_util[i] = ((double)(100 * diff))/1000000000;
       cpu_stats[i]= max(0.0,min(100.0,cpu_util[i]));
      printf("\n %s -> CPU utilization : %lf \n",domname,cpu_stats[i]);
          if(cpu_util[i]>max_cpu){
            
                if(high_count[i]>=2)
                {
                  high_count[i]=0;
                  low_count[i]=0;
                  if(spawn_vm_index < MAX_DOMAINS){
                       if(spawn_new_vm==0) spawn_new_vm=1;
                   }
                   else{
                     printf("Max VM limit reached");
                                            char msg[40]="Max VM limit reached";
                                            strcpy(msg_content,msg);
                                            display_msg=1; 
   // gdk_threads_add_idle(quick_message,msg);
                   }
                 }
                 if(spawn_new_vm ==0)
                 high_count[i]++;
              }
             else if (cpu_util[i]<min_cpu){
              
               
               if(low_count[i]>=4 && virConnectNumOfDomains(conn)>1 && spawn_new_vm==0){
                     
                     low_count[i]=0;
                     high_count[i]=0;
                          const char * dom_name=virDomainGetName(dom_ptr[i]);
                    if(dom_name==NULL)
                     {
                       for(int i=0;i<MAX_DOMAINS;i++){
                        low_count[i]=0;
                       }
                      //post_destroy_task(NULL,1);
                      break;
                     }
                   // const char * dom_name = virDomainGetName(dom_ptr[i]);
                    //post_destroy_task(dom_name,0);
                  for(int i=0;i<MAX_DOMAINS;i++){
                               low_count[i]=0;
                        }
                   //  active_vm_num--;
                    //  sleep(5);
                     if(virDomainDestroy(dom_ptr[i])==-1){

                       printf("VM error found");
                        for(int i=0;i<MAX_DOMAINS;i++){
                               low_count[i]=0;
                        }
                       //post_destroy_task(NULL,1);
                       break;
                     }
                    
                     printf("%s shutting down due to low load\n",dom_name);
                           char msg[40]="Shutting down ";
                           strcat(msg,dom_name);
                           strcat(msg," due to low load");
                           strcpy(msg_content,msg);
                       display_msg=1;  
                  // gdk_threads_add_idle(quick_message,msg);
                    //s printf("%s shut down\n",dom_name);
                 
                    
                     
                     
                     
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
            // printf("Reached end of for loop %d\n",i);
      }
       //update_gui();


   
   }
  else{
    printf("Error while getting cpu stats\n");
  }

  GtkListStore  *store;
  GtkTreeIter    iter;
  virTypedParameter params;
   virDomainPtr *dom_ptr;
  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING,G_TYPE_DOUBLE);
virDomainStatsRecordPtr *retStats;
int temp=0;
int first_time=0;

int ret=virConnectListAllDomains(conn, &dom_ptr, VIR_CONNECT_LIST_DOMAINS_PERSISTENT);
   

   for(int i=0;i<ret;i++){
  
 
    char * vm_name_temp= (char *) virDomainGetName(dom_ptr[i]);
     vm_detail[i].server_name = vm_name_temp;
     int state,reason=0;
     virDomainGetState(dom_ptr[i],&state,&reason,0);
     switch (state)
     {
     case 1: 
              if(spawn_new_vm==0)
              {
                if(first_time==0)
                {
                  first_time=1;
                  vm_detail[i].ip = get_ip_addr(dom_ptr[i],1,1);
                }
                else{
                    vm_detail[i].ip = get_ip_addr(dom_ptr[i],0,1);
                }
              }
            vm_detail[i].status="Active";
            vm_detail[i].cpu=cpu_stats[temp++];
     break;
     
     default: 
         vm_detail[i].ip = NULL;
            vm_detail[i].status="InActive";
            vm_detail[i].cpu=0;
       break;
     }
  
         
          GtkLabel *label_tmp;
          label_tmp=GTK_LABEL(label[i][0]);
          gtk_label_set_text(label_tmp,vm_detail[i].server_name);
           label_tmp=GTK_LABEL(label[i][1]);        
          gtk_label_set_text(label_tmp,vm_detail[i].ip);
           label_tmp=GTK_LABEL(label[i][2]);
          gtk_label_set_text(label_tmp,vm_detail[i].status);

          char cpu_utilization[sizeof(vm_detail[i].cpu)];
           sprintf(cpu_utilization,"%lf",vm_detail[i].cpu);
           label_tmp=GTK_LABEL(label[i][3]);
          gtk_label_set_text(label_tmp,cpu_utilization);
          
        
                      
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

void * initialize()
{
    
    
    virSetErrorFunc(NULL, customErrorFunc);
virDomainPtr dom;
char *ip=NULL;
pthread_t pid_t;
  struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
    

   // initialize_grid();
    if(get_active_vm()>0){
      pthread_create(&pid_t,NULL,&get_cpu_utilization,NULL);
    }
    else{
    if(get_an_inactive_vm(conn) == NULL){
        printf("No active VM found ...exiting");
            virConnectClose(conn);
        return NULL;
    }
  
    dom=virDomainLookupByName(conn, get_an_inactive_vm(conn));
    

    if (!dom) {
        fprintf(stderr, "Unable to load guest configuration");
        return NULL;
    }

    if (virDomainCreate(dom) < 0) {
        fprintf(stderr, "Unable to boot guest configuration");
        return NULL;
    }
    printf("Guest %s is booting up\n",inactive_vm[0]);
     
                           char msg[40]="Guest ";
                           strcat(msg,inactive_vm[0]);
                           strcat(msg," is booting up");
                      
    gdk_threads_add_idle(quick_message,msg);

    vm_name[0]=inactive_vm[0];
    vm_index++;
   // free(inactive_vm); 
    
    sleep(vm_wait);

    // ip = get_ip_addr(dom,1,1);
    // if(ip !=NULL){
    //   printf("ip address: %s\n",ip);
    //   vm_detail[0].ip=ip;
    // }
    // active_vm[0]=inactive_vm[0];
    // active_vm_num=1;
   // update_gui();
    pthread_create(&pid_t,NULL,&get_cpu_utilization,NULL);
    } 
    
    
   while(1){
   

       
        if(spawn_new_vm==1){
         

        if(create_new_VM(vm_to_start)!=NULL)
        {
                           char msg[50]="Guest ";
        
        (vm_to_start==NULL)?strcat(msg,inactive_vm[0]):strcat(msg,vm_to_start);
                            strcat(msg," is booting up"); 
          if(spawn_new_vm==1) gdk_threads_add_idle(quick_message,msg);
    
         sleep(vm_wait);   
         strcpy(msg,"");
          strcat(msg,"Guest ");
         // strcat(msg,vm_to_start);
           (vm_to_start==NULL)?strcat(msg,inactive_vm[0]):strcat(msg,vm_to_start);
          strcat(msg," has successfully booted up"); 
             if(spawn_new_vm==1) gdk_threads_add_idle(quick_message,msg);
        }
      
        vm_to_start=NULL;
         spawn_new_vm =0;
     
       
      }
           if(display_msg==1){
        display_msg=0;
                      
    gdk_threads_add_idle(quick_message,msg_content);

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

    
}