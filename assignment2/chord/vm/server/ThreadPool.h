#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib> 
#include <iostream>
#include <unistd.h>
#include <thread>
#include <vector>
#include <queue>
#include "Node.h"
#include <mutex>
#include <string.h>
#include <condition_variable>
#include "KVClientHandler.h"
#include "HelperFunctions.h"
#include "KVStore.h"
#include "KVCache.hpp"
#include <iostream>
#include <functional>
#define MAX_KEY_LENGTH 257
#define MAX_VALUE_LENGTH 262145

using namespace std;
//int counter=0;
extern pthread_mutex_t* lock_main;
extern pthread_cond_t* cv;
extern int* cv_arr;
extern int thread_pool_size;
extern Node node;
class ThreadPool{	
public:
	

	ThreadPool(){
		

		done=false;
		if(thread_pool_size==0)
		{
			thread_pool_size=1;
		}
		for(int i=0;i<100;i++){
			pthread_mutex_init(&lock_file[i],NULL);
			pthread_cond_init(&cv_file[i], NULL);
		}

		for(int i=0;i<thread_pool_size;i++){
		
			threads.push_back(thread(&ThreadPool::assign_work_to_thread,this));
		}

	}

	~ThreadPool(){
		done=true;
		cond_wait.notify_all();
		for(auto& thread: threads){
			if(thread.joinable()){
				thread.join();
			}
		}

	}

	void add_work_to_job_pool(int fd,string &xml){
		lock_guard<mutex> my_mutex(work_mutex);

		jobqueue.push(pair<int,string>(fd,xml));
		cond_wait.notify_one();
	}
private:
	condition_variable_any cond_wait;
	vector<thread> threads;

	mutex work_mutex;
	queue<pair<int,string>> jobqueue;
	pthread_mutex_t lock_file[100];
pthread_cond_t cv_file[100];
	bool done;
	void assign_work_to_thread(){
		while(!done){
			pair<int,string> input;
			{
				unique_lock<mutex> cond_lock(work_mutex);
				cond_wait.wait(cond_lock,[&]{
					return !jobqueue.empty() || done;
				});
				input=jobqueue.front();
				jobqueue.pop();
			}
			worker_thread(input);
		}
	}
	int write_response(int sock_fd,char resp[]){
			int count=-1;
		//	try{
			count=send(sock_fd,resp,strlen(resp), MSG_NOSIGNAL);
			if(count == -1){
				printf("Error while writing");
			}
			// }
			// catch(...){
			// 	cout << "\nException while writing\n";
			// }
			return count;
	
	}
	

	string generate_response(char resp_type[],char key_par[],char value_par[],char error_msg[]){
	string genxml_1="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"resp\">\n<Key>keyrep</Key>\n<Value>valrep</Value>\n</KVMessage>";
	string genxml_2="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"resp\">\n<Message>Success</Message>\n</KVMessage>";
	string resp(resp_type);
	string key(key_par);
	string value(value_par);
	string error(error_msg);
	string modified_xml="Error";
	if((resp.compare("put")==0))
	{	
		
		return genxml_2;
		
		
	}
	else if (resp.compare("error")==0)
	{

		genxml_2.replace(72,7,error_msg);

		return genxml_2;
		
	}
	else if (resp.compare("get")==0)
	{
		

		genxml_1.replace(88,6,value);
		genxml_1.replace(68,6,key);
		return genxml_1;

	}
    else if(resp.compare("del")==0){
    	if(strcmp(error_msg,"")!=0)
    	genxml_2.replace(72,7,error_msg);
		return genxml_2;
    }
	
	
  	return modified_xml;
}

int get_file_set_Id(string key){
	int sum=0;
	for(int i=0; key[i]!='\0'; i++){
		sum += key[i];
	}
	return sum % 100;
}
/*
void timer_start(std::function<void(void)> func, unsigned int interval)
{
  std::thread([func, interval]()
  { 
    while (true)
    { 
      auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
      func();
      std::this_thread::sleep_until(x);
    }
  }).detach();
}*/

	void worker_thread(const pair<int,string> input){
	//	std::cout << "From Thread ID : "<<std::this_thread::get_id() << " locking \n";
   // try{
		int set,fileset;
		string key=string();
		string value=string();
		string command=string();
		int fd = input.first;
		string temp=input.second;
		bool cs_in_use=false;
		char xml[temp.length()+1]; 
 	    strcpy(xml,temp.c_str());
		parse_xml(command,key,value,xml);


			key.erase(remove(key.begin(), key.end(), '\n'), key.end());
	key.erase(remove(key.begin(), key.end(), '\r'), key.end());
		value.erase(remove(value.begin(), value.end(), '\n'), value.end());
	value.erase(remove(value.begin(), value.end(), '\r'), value.end());
char temp_n[]="";
			if(key.length()>MAX_KEY_LENGTH)
		{
			char par1[]="error";
				char par4[]="Oversized key";
				string temp=generate_response(par1,temp_n,temp_n,par4);
				char resp[temp.length()+1];
					strcpy(resp,temp.c_str());
				write_response(input.first,resp);
				return;
		}
		if(value.length()>MAX_KEY_LENGTH)
		{
			char par1[]="error";
				char par4[]="Oversized value";
				string temp=generate_response(par1,temp_n,temp_n,par4);
				char resp[temp.length()+1];
					strcpy(resp,temp.c_str());
				write_response(input.first,resp);
				return;
		}
		string put_req="\"putreq\"";
		string get_req="\"getreq\"";
		
		
		string del_req="\"delreq\"";
		string suc_req="\"getsuc\"";
		
		fileset = get_file_set_Id(key);
		set=getSetId(key);
		//timer_start(create_xml,100000);

		//lock
		pthread_mutex_lock(&lock_main[set]);
		pthread_mutex_lock(&lock_file[fileset]);
		if(cs_in_use)
		{
		
		pthread_cond_wait(&cv_file[fileset],&lock_file[fileset]);
		pthread_cond_wait(&cv[set],&lock_main[set]);
		}
		cs_in_use=true;
		HelperFunctions help;
		std::cout << "From Thread ID : "<<std::this_thread::get_id() << "\n";
	    cout <<"KV --> command = " << command << " key=" << key << "Value=" << value <<endl;
	//	cout <<"\ncommand = " << command << " key=" << key << "Value=" << value <<endl;
    if(command.compare("\"getsuc\"")==0){
 
       if(help.check_string_for_number(key)==false){
           char resp[]="-1";
            write_response(fd,resp);
            
       }
       else{
		   	node_info succ_node;
	  try{
         succ_node = node.findSuccessor(stoi(key));
	  }
	  catch(...){
		  string data = "-1";
		      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
	  }
      char ip[succ_node.ip.size()+1];
      strcpy(ip,succ_node.ip.c_str());
      string data = help.combine(ip,succ_node.port);
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
     // close(fd);
      
       }
      
    }
    else if(command.compare("\"sendSuccList\"")==0){

      string data = node.get_string_from_node_list();
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
     
    }
    else if(command.compare("\"getSuccId\"")==0){
      
      string data = to_string(node.successor.id);
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
     
    }
    else if(command.compare("\"pred\"")==0){
      string data;
      if(node.predecessor.ip ==""){
        data = "";      
      }
      else{
        char ip[node.predecessor.ip.size()+1];
        strcpy(ip,node.predecessor.ip.c_str());
        data = help.combine(ip,node.predecessor.port);
      }
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
      
    }
    else if(command.compare("\"predStblz\"")==0){
      string data;
      if(node.predecessor.ip ==""){
        data = "no_pred";      
      }
      else{
        char ip[node.predecessor.ip.size()+1];
        strcpy(ip,node.predecessor.ip.c_str());
        data = help.combine(ip,node.predecessor.port);
      }
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
      node_info node_detail = node.getIpPort(value);
      node_detail.id = help.find_consitant_hash_id(value);
      node.notify(node_detail);
      
    }
    else if(command.compare("\"heart_beat\"")==0){
      
      string data = "alive";
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
     
    }
    else if(command.compare("\"getKeyValueSet\"")==0){
      
      string data = help.get_key_value_set();
      char resp[data.length()+1];
			strcpy(resp,data.c_str());
      write_response(fd,resp);
      //close(fd);
    }
	else if(command==""){
		//close(fd);
	}
    else{
	
		int key_hash = help.find_consitant_hash_id(key);
		string temp;
		string orig_cmd = command;
        if((temp=node.request_from_same_node(command,node.predecessor))!=""){
			node.from_pred=true;
			command = temp;
		}
        node_info node_succ = node.findSuccessor(key_hash);

		if(node_succ.id != node.current.id && node_succ.ip!=""&& node_succ.port!=-1 && node.request_from_same_node(orig_cmd,node_succ)!=""){
			 
			command = command.substr(1,command.size()-2);
			command += ":" + node.current.ip + ":" + to_string(node.current.port);
			char command_cstr[command.size()+1];
			char key_cstr[key.size()+1];
			char value_cstr[value.size()+1];
			strcpy(command_cstr,command.c_str());
			strcpy(key_cstr,key.c_str());
			
			strcpy(value_cstr,value.c_str());
			
			string input_str = convert_to_xml(command_cstr,key_cstr,value_cstr);

			string resp = node.establish_connection_and_forward_msg(node_succ.ip,node_succ.port,input_str);
        	char resp_chr[resp.size()+1];
        	strcpy(resp_chr,resp.c_str());
        	write_response(input.first,resp_chr);
		}
		else{
		//	cout << "\n Perfoming operation on current node";
		if(command==put_req){
			
			int status=put(key,value);
			if(status==1)
			{
				update_synchronously(key,value,fileset);
				char par1[]="put";
				string temp=generate_response(par1,temp_n,temp_n,temp_n);
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());
				write_response(input.first,resp);
			}
			else
			{
			if(read_sychronously(key,fileset)=="Does not exist"){
				write_asynchronously(key,value,fileset);
				char par1[]="put";

				string temp=generate_response(par1,temp_n,temp_n,temp_n);
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());

				write_response(input.first,resp);
			}
			else{
				update_synchronously(key,value,fileset);
				char par1[]="put";
				string temp=generate_response(par1,temp_n,temp_n,temp_n);
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());
				write_response(input.first,resp);
				
				
			}
		
		  }

			
		
		}
		else if(command==get_req){
			string cache_value=get(key);
			if(cache_value=="miss"){

					string send_temp=read_sychronously(key,fileset);
					if(send_temp!="Does not exist")
					{
					put(key,send_temp);
				}
			char send_data[send_temp.length()];
			strcpy(send_data,send_temp.c_str());
			char key_temp[key.length()+1];
			strcpy(key_temp,key.c_str());
			char par1[]="get";
			string temp=generate_response(par1,key_temp,send_data,temp_n);
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());

			write_response(input.first,resp);
			}
			else{
				//
				char send_data[cache_value.length()];
			strcpy(send_data,cache_value.c_str());
			char key_temp[key.length()+1];
			strcpy(key_temp,key.c_str());
			char par1[]="get";
			string temp=generate_response(par1,key_temp,send_data,temp_n);
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());

			write_response(input.first,resp);
			}
		
		
			
		}
		else if(command==del_req){
			remove(key);
			string temp;
			char par1[]="del";
			string send_temp=read_sychronously(key,fileset);
					if(send_temp=="Does not exist")
					{
						char par4[]="Does not exist";
				
				temp=generate_response(par1,temp_n,temp_n,par4);

				}
				else{
					delete_synchronously(key,fileset);
					temp=generate_response(par1,temp_n,temp_n,temp_n);
			}
				
				
			
			
				char resp[temp.length()+1];
				strcpy(resp,temp.c_str());

			write_response(input.first,resp);
		}
	
		else{
				char par1[]="error";
				char par4[]="Input not in correct format";
				string temp=generate_response(par1,temp_n,temp_n,par4);
				char resp[temp.length()+1];
					strcpy(resp,temp.c_str());
							write_response(input.first,resp);
		}
	   }
	}
		//std::cout << "From Thread ID : "<<std::this_thread::get_id() << " worker thread work done \n";

		
		//counter++;
		//printf("%d\n",counter);
		//print();
		cs_in_use=false;
		pthread_cond_signal(&cv_file[fileset]);
		pthread_cond_signal(&cv[set]);		
		pthread_mutex_unlock(&lock_file[fileset]);
		pthread_mutex_unlock(&lock_main[set]);

	//	std::cout << "From Thread ID : "<<std::this_thread::get_id() << " unlocked \n";





	// }
	//  catch(...){
    //     cout << "Exception occured";
    // }
	}
      
};

