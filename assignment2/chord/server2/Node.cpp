#include "Node.h"
Node::Node(){
   present=false;
   /****/
   node_info node;
   node.ip="";
   node.port=-1;
   node.id=-1;
   from_pred=false;
    for(int i=1;i<=key_space;i++){
       
        succesorList[i]=node;
        finger_table[i]=node;
        
    }
  

}


void Node::setSuccessorList(node_info node){
    for(int i=1;i<=key_space;i++){
       
        succesorList[i]=node;
        
    }
}

void Node::setFingerTable(node_info node){

   for(int i=1;i<=key_space;i++){
       
        finger_table[i]=node;
    }

}
struct node_info Node::findSuccessor(int nodeId){


    /****/
   //cout << "findSuccessor("<<nodeId<<")"<<endl;
   node_info node;
   node.id=current.id;
   node.ip=current.ip;
   node.port=current.port;
   /** check whether the node id lies between the current and the successor node **/
   if(nodeId > current.id && nodeId <= successor.id){
       return successor;
   }
   else if(successor.id==current.id||nodeId==current.id){
       return node;
   }
   else if(successor.id ==predecessor.id){
       if(successor.id>current.id){
           if(nodeId>successor.id || nodeId<current.id){
               return node;
           }
       }
       else{
           if((nodeId>current.id && nodeId >successor.id)||nodeId<current.id && nodeId < successor.id){
               return successor;
           }
           else{
               return node;
           }
       }
   }
   else{
      /****/
       
       node_info node_closest =closestPrecedingNode(nodeId);
       if(node_closest.port==current.port && node_closest.ip == current.ip){
           return successor;
       }
       else{
           if(node_closest.id==-1){
               node=successor;
           }

           node_closest.id = nodeId;
           
            string rec_msg;
             if(from_pred==true){
                 from_pred=false;
                 return current;
             }
             rec_msg = establish_connection_and_sendmsg(node_closest,"getsuc");
           
           if(rec_msg.empty()||rec_msg=="-1"){
             node_info node;
             node.ip="";
             node.port=-1;
             node.id=-1;
             return node;
           }

           HelperFunctions helper;
           int hash = helper.find_consitant_hash_id(rec_msg);
           node_info node_succ = getIpPort(rec_msg);
           node_succ.id = hash;
           return node_succ;
           
           


       }
   }
   node.ip="";
   node.id=-1;
   node.port = -1;
   return node;

}
struct node_info Node::closestPrecedingNode(int  nodeId){
    /***/
  // cout << "closestPrecedingNode("<<nodeId<<")"<<endl;
   for(int i=key_space;i>=1;i--){
       if(finger_table[i].ip == "" || finger_table[i].id ==-1 || finger_table[i].port==-1){
           continue;
       }
       if(finger_table[i].id > current.id && finger_table[i].id<nodeId){
           return finger_table[i];

       }
       else{

           int suc_id = getSuccessorID(finger_table[i].ip,finger_table[i].port);
           if (suc_id ==-1){
               continue;
           }

           if(finger_table[i].id > suc_id){
               if((nodeId <= finger_table[i].id && nodeId <=suc_id) || (nodeId>=finger_table[i].id && nodeId>= suc_id)){
                    return finger_table[i];
               }
           }
           else if(finger_table[i].id <suc_id&& nodeId > finger_table[i].id && nodeId < suc_id){
               return finger_table[i];
           }

           node_info node = getPredecessorNode(finger_table[i].ip,finger_table[i].port,"",-1,false);
           int pred_id = node.id;
           if(pred_id !=-1 && finger_table[i].id < pred_id){
               if((nodeId <= finger_table[i].id && nodeId <= pred_id)|| (nodeId >=finger_table[i].id && nodeId >= pred_id)){
                   return node;
               }
           }
           if(pred_id != -1 && finger_table[i].id > pred_id&& nodeId >= pred_id && nodeId <= finger_table[i].id){
               return node;
           }
       }
   }
   node_info node;
   node.id=-1;
   node.ip="";
   node.port = -1;
   return node;
}

void Node:: stabilize(){
   // cout << "stabilize"<<endl;
    if(check_node_status(successor.ip,successor.port)==false){
        return;
    }

    node_info node = getPredecessorNode(successor.ip,successor.port,current.ip,current.port,true);
    int pred_hash = node.id;
    if(pred_hash ==-1 || predecessor.id ==-1){
        return;
    }

    if(pred_hash > current.id || (pred_hash >current.id && pred_hash < successor.id)||(pred_hash<current.id && pred_hash < successor.id)){
        successor = node;
    }
}

void Node:: checkPredecessor(){
   // cout << "CheckPredecessor("<<endl;
    if(predecessor.id ==-1){
        return;
    }
    string pred_ip=predecessor.ip;
    int pred_port = predecessor.port;
    if(check_node_status(pred_ip,pred_port)==false){
        if(predecessor.id ==successor.id){
            successor.id = current.id;
            successor.ip = current.ip;
            successor.port = current.port;
            setSuccessorList(successor);
        }
      predecessor.ip="";
      predecessor.id=-1;
      predecessor.port=-1;
    }
}

void Node::checkSuccessor(){
   // cout << "checkSuccessor"<<endl;
    if(successor.id == current.id){
        return;
    }

    string ip = successor.ip;
    int port = successor.port;
    if(check_node_status(ip,port)==false){
        successor = succesorList[2];
        /***/
        updateSuccessorList();
    }
}

void Node::notify(node_info node){
   // cout << "notify"<<endl;
    int pred_id = predecessor.id;
    int node_id = node.id;
    predecessor = node;
    if(successor.id ==current.id){
        successor = node;
    }
}
void Node::fixFingers(){
  //  cout << "fixFIngers"<<endl;
    int mod = pow(2,key_space);
    for(int i=1;i<=key_space;i++){
        if(check_node_status(successor.ip,successor.port)==false)
        {
            return;
        }
        int newId = current.id +pow(2,i -1);
        newId = newId % mod;
        node_info node = findSuccessor(newId);
       if(node.id == -1 || node.port ==-1 || node.ip =="")
       {
           break;

       }
       finger_table[i]=node;

    }
    
}

void Node::updateSuccessorList(){
    /***/
  //  cout << "UpdateSuccessorList"<<endl;
    vector<node_info> node_list = getSuccessorListFromNode(successor.ip,successor.port);
    if(node_list.size() != key_space){
        return;
    }
    succesorList[1]=successor;
    for(int i=2;i<=key_space;i++){
        succesorList[i].id = node_list[i-1].id;
        succesorList[i].ip = node_list[i-1].ip;
        succesorList[i].port = node_list[i-1].port;
    }
}

vector<node_info> Node::getSuccessorListFromNode(string ip,int port){

    node_info node;
    node.ip=ip;
    node.port=port;
    /***/
    HelperFunctions help;
    char ip_chr[ip.size()+1];
    strcpy(ip_chr,ip.c_str());
    node.id = help.find_consitant_hash_id(help.combine(ip_chr,port));
    vector<node_info> node_list;

    string succ_list =  establish_connection_and_sendmsg(node,"sendSuccList");
    if(succ_list==""){
        return node_list;
    }
     node_list= get_node_list_from_string(succ_list);
    return node_list;
}

int Node::getSuccessorID(string ip,int port){
    //cout << "getSuccessorID("<<ip<<","<<port<<")"<<endl;
    node_info node;
    node.ip=ip;
    node.port=port;
    HelperFunctions help;
    char ip_chr[ip.size()+1];
    strcpy(ip_chr,ip.c_str());
    node.id = help.find_consitant_hash_id(help.combine(ip_chr,port));
    if(node.ip == current.ip && node.port==current.port){
        return successor.id;
    }

    string succ_id = establish_connection_and_sendmsg(node,"getSuccId");
    if(succ_id ==""){
        return -1;
    }
   
    if(help.check_string_for_number(succ_id)==false){
      cout << succ_id;
      return -1;
    }
    return stoi(succ_id);
}

string Node::establish_connection_and_sendmsg(node_info node,string msg_str){
    // cout << "establist connection and send msg("<<node.id<<","<<node.ip<<","<<node.port<<","<<msg_str<<")"<<endl;
    if(node.ip == "" || node.port==-1||node.id == -1){
        return "";
    }
    if(node.ip == current.ip && node.port == current.port){
        return "";
    }
	char buffer[1000];
	int count; 
      
	struct sockaddr_in server_address; 
    int sock_fd,check; 
    struct hostent *hostdetails;



	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    printf("Network Error: Could not create socket\n");
        return "";
	    //exit(1);
	}
    hostdetails = gethostbyname(node.ip.c_str());
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(node.port);  
        if(hostdetails->h_addr ==NULL)
    {
        return "";
    }
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    printf("Network Error: Could not connect\n");
        close(sock_fd);
        return "";

	    //exit(1);
	}



    HelperFunctions help;
	char hostid[10];
    sprintf(hostid,"%d", node.id);
	char iptemp[node.ip.size()+1];

	strcpy(iptemp,node.ip.c_str());
    string ip_port_str = help.combine(iptemp,node.port);

    char ip_port[ip_port_str.size()+1];
    strcpy(ip_port,ip_port_str.c_str());	
    char msg[msg_str.size()+1];
    strcpy(msg,msg_str.c_str());
	string buffer_w=convert_to_xml(msg,hostid,ip_port);
	char send_char_data[buffer_w.length()+1]; 
 	strcpy(send_char_data,buffer_w.c_str());  
 	check=send(sock_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
     string ret= "";
 	if(check==-1){
			printf("Network Error: Could not send data\n");
            close(sock_fd);
            return "";
		    
	}
    char resp[10000];
    memset(resp,'\0',10000);

    FD_ZERO(&set);
   FD_SET(sock_fd,&set);
   timeout.tv_sec = 0;
   timeout.tv_usec = 100000;
   int rv = select(sock_fd+1,&set,NULL,NULL,&timeout);
	if(rv == -1|| rv ==0)
     {
      close(sock_fd);
      return ret;
     }
  	else
      {
        count=read(sock_fd, resp, sizeof(resp));
      }
    if(count!=-1){
        ret = resp;
    }
 
	close(sock_fd);
    return ret;
}

string Node::establish_connection_and_sendmsg_special(node_info node,string msg_str,string value){
     //cout << "establist connection and send msg("<<node.id<<","<<node.ip<<","<<node.port<<","<<msg_str<<")"<<endl;
	if(node.ip == "" || node.port==-1||node.id == -1){
        return "";
    }
        if(node.ip == current.ip && node.port == current.port){
        return "";
    }
	char buffer[1000];
	int count; 
      
	struct sockaddr_in server_address; 
    int sock_fd,check; 
    struct hostent *hostdetails;



	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    printf("Network Error: Could not create socket\n");
	    return "";
        //exit(1);
	}
    hostdetails = gethostbyname(node.ip.c_str());
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(node.port);  
        if(hostdetails->h_addr ==NULL)
    {
        return "";
    }
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    printf("Network Error: Could not connect\n");
        close(sock_fd);
	    return "";
        //exit(1);
	}



    HelperFunctions help;
	char hostid[10];
    sprintf(hostid,"%d", node.id);
	char iptemp[node.ip.size()+1];

	strcpy(iptemp,node.ip.c_str());
  //  string ip_port_str = help.combine(iptemp,node.port);

    char ip_port[value.size()+1];
    strcpy(ip_port,value.c_str());	
    char msg[msg_str.size()+1];
    strcpy(msg,msg_str.c_str());
	string buffer_w=convert_to_xml(msg,hostid,ip_port);
	char send_char_data[buffer_w.length()+1]; 
 	strcpy(send_char_data,buffer_w.c_str());  
 	check=send(sock_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
     string ret= "";
 	if(check==-1){
			printf("Network Error: Could not send data\n");
            close(sock_fd);
            return "";
		    
	}
    char resp[10000];
    memset(resp,'\0',10000);
     FD_ZERO(&set);
   FD_SET(sock_fd,&set);
   timeout.tv_sec = 0;
   timeout.tv_usec = 100000;
   int rv = select(sock_fd+1,&set,NULL,NULL,&timeout);
	if(rv == -1|| rv ==0)
     {
      close(sock_fd);
      return ret;
     }
  	else
      {
        count=read(sock_fd, resp, sizeof(resp));
      }
    if(count!=-1){
        ret = resp;
    }
 
	close(sock_fd);
    return ret;
}
struct node_info Node::getIpPort(string ip_port){
    int pos = ip_port.find_first_of(":");
    string ip = ip_port.substr(0,pos);
   
    HelperFunctions help;
    int port=0;
     node_info node;
    if(help.check_string_for_number(ip_port.substr(pos+1))){
        
    port = stoi(ip_port.substr(pos+1));
     node.ip = ip;
    node.port = port;
    }
    else{
        cout<< ip_port.substr(pos+1);
         node.ip = "";
    node.port = -1;
    }
   
   
    return node;
}
struct node_info Node::getPredecessorNode(string ip,int port,string c_ip,int c_port,bool stblz){
     //cout << "getPredecessorNode("<<ip<<","<<port<<")"<<endl;
    string msg = "";
    HelperFunctions help;
    if(ip==current.ip && port == current.port){
      return predecessor;
    }
    node_info node;
    string resp;
    if(stblz){
        
        msg = "predStblz";
  
    }
    else{
        msg = "pred";
    }
    if(c_ip =="" || c_port ==-1){
        node.ip=ip;
        node.port=port;
        char str[ip.size()];
        strcpy(str,ip.c_str());
        node.id = help.find_consitant_hash_id(help.combine(str,port));
        resp= establish_connection_and_sendmsg(node,msg);
    }
    else{
        node.ip=ip;
        node.port=port;
        char str[ip.size()];
        strcpy(str,ip.c_str());
        node.id = help.find_consitant_hash_id(help.combine(str,port));
        char c_ip_str[c_ip.size()];
        strcpy(c_ip_str,c_ip.c_str());
        string value = help.combine(c_ip_str,c_port);

        resp= establish_connection_and_sendmsg_special(node,msg,value);
    }

    if(resp =="no_pred" || resp ==""){
        node.ip = "";
        node.id=-1;
        node.port=-1;
    }
    else{
        node = getIpPort(resp);
        node.id = help.find_consitant_hash_id(resp);
    }
    return node;
}
bool Node::check_node_status(string ip,int port){
    node_info node;
    node.ip = ip;
    node.port=port;
    HelperFunctions help;
    char ip_char[ip.size()+1];
    strcpy(ip_char,ip.c_str());
    node.id= help.find_consitant_hash_id(help.combine(ip_char,port));
    string resp =  establish_connection_and_sendmsg(node,"heart_beat");
    if(resp=="alive"){
        return true;

    }
    else{
        return false;
    }
}
vector<node_info> Node::get_node_list_from_string(string suc_list){
    
     vector<node_info> list;
     node_info node;
     char c_suc_list[suc_list.size()+1];
     strcpy(c_suc_list,suc_list.c_str());
     char * c_ptr= c_suc_list;
     char *tmp;
     HelperFunctions help;
    
     while(tmp = __strtok_r(c_ptr,";",&c_ptr)){
           string ipport= tmp;
           node = getIpPort(ipport);
           node.id = help.find_consitant_hash_id(ipport);
           list.push_back(node);
            
     }
     return list;


}
string Node::get_string_from_node_list(){
    string ret;
    for(int i=1;i<=key_space;i++){
        ret = ret + succesorList[i].ip + ":" + to_string(succesorList[i].port) + ";";

    }
    return ret;
}
void Node::print_data(){
    cout << "Current -> "<<current.id;
    cout << " : "<<current.port;
    cout << " : " << current.ip <<"\n";
    cout << "Successor ->";
    cout <<successor.id;
    cout << " : "<<successor.port;
    cout << " : " << successor.ip <<"\n";
    cout << "Precessor ->";
    cout <<predecessor.id;
    cout << " : "<<predecessor.port;
    cout << " :" << predecessor.ip <<"\n";
    cout << "****Fingertable*****\n";

    for(int i=1;i<=key_space;i++){

        cout <<finger_table[i].id <<":";
        cout <<finger_table[i].ip <<":";
        cout <<finger_table[i].port <<"\n";
    }

}

void Node::getDataFromSuccessor(string ip,int port){
    node_info node;
    node.ip=ip;
    node.port=port;
    HelperFunctions help;
    char ip_str[ip.size()+1];
    strcpy(ip_str,ip.c_str());
    node.id = help.find_consitant_hash_id(help.combine(ip_str,port));
    string key_value_set =establish_connection_and_sendmsg(node,"getKeyValueSet");
    if(key_value_set!="")
    {
    help.store_key_value_set(key_value_set);
    }
    
}
string Node::establish_connection_and_forward_msg(string ip,int port,string msg){
    // cout << "establist connection and forward message("<<ip<<","<<port<<","<<msg<<")"<<endl;
    if(ip == "" || port==-1){
        return "";
    }
    if(ip == current.ip && port == current.port){
        return "";
    }
	char buffer[1000];
	int count; 
      
	struct sockaddr_in server_address; 
    int sock_fd,check; 
    struct hostent *hostdetails;



	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    printf("Network Error: Could not create socket\n");
        return "";
	    //exit(1);
	}
    hostdetails = gethostbyname(ip.c_str());
	server_address.sin_family = AF_INET;    
	server_address.sin_port = htons(port);  
        if(hostdetails->h_addr ==NULL)
    {
        return "";
    }
	server_address.sin_addr = *((struct in_addr *)hostdetails->h_addr);
	bzero(&(server_address.sin_zero),8);

	if (connect(sock_fd, (struct sockaddr *)&server_address,sizeof(server_address)) == -1)
	{
	    printf("Network Error: Could not connect\n");
        close(sock_fd);
        return "";

	    //exit(1);
	}

	char send_char_data[msg.length()+1]; 
 	strcpy(send_char_data,msg.c_str());  
 	check=send(sock_fd,send_char_data,strlen(send_char_data), MSG_NOSIGNAL);
     string ret= "";
 	if(check==-1){
			printf("Network Error: Could not send data\n");
            close(sock_fd);
            return "";
		    
	}
    char resp[10000];
    memset(resp,'\0',10000);
     FD_ZERO(&set);
   FD_SET(sock_fd,&set);
   timeout.tv_sec = 0;
   timeout.tv_usec = 100000;
   int rv = select(sock_fd+1,&set,NULL,NULL,&timeout);
	if(rv == -1|| rv ==0)
     {
      close(sock_fd);
      return ret;
     }
  	else
      {
        count=read(sock_fd, resp, sizeof(resp));
      }
    if(count!=-1){
        ret = resp;
    }
 
	close(sock_fd);
    return ret;
}
string Node::request_from_pred(string command_param){
    if(command_param==""){
        return "";
    }
    
        int pos = command_param.find_first_of(":");
        if(pos !=-1){
        string command = command_param.substr(0,pos);
        command += "\"";
             
        string ip_port = command_param.substr(pos+1,command_param.size() - (pos + 2));
        node_info node = getIpPort(ip_port);
        if(node.ip == predecessor.ip && node.port==predecessor.port){
            return command;
        }
        
           
        
    }
     return "";
}
