#ifndef Node_h
#define Node_h
#include <iostream>
#include <vector>
#include <map>
#include<unistd.h>
#include"HelperFunctions.h"
using namespace std;
#define key_space 16
   struct node_info{
     int id;
         string ip;
         int port;
     };
class Node{
 
     public:
    Node();
 
     bool present;
     
     struct node_info predecessor;
     struct node_info successor;
     struct node_info current;
     struct node_info finger_table[key_space+1];
     struct node_info succesorList[key_space+1];
     fd_set set;
     struct timeval timeout;
     node_info findSuccessor(int);
      node_info closestPrecedingNode( int);
      void fixFingers();
      void stabilize();
      void notify(node_info);
      void checkPredecessor();
      void checkSuccessor();
      void updateSuccessorList();
      
      void setSuccessorList(node_info node);
      
      void setFingerTable(node_info node);
      
       int getSuccessorID(string ip,int port);

       string establish_connection_and_sendmsg(struct node_info node,string msg);
        node_info getIpPort(string);

        node_info getPredecessorNode(string ip,int port,string c_ip,int c_port,bool stblz);
        bool check_node_status(string ip,int port);
        vector<node_info> getSuccessorListFromNode(string ip,int port);
        string establish_connection_and_sendmsg_special(node_info node,string msg_str,string value);
        vector<node_info> get_node_list_from_string(string);
        string get_string_from_node_list();
        void print_data();
        void getDataFromSuccessor(string ip,int port);
        vector<string> getKeyValue(string);
        string establish_connection_and_forward_msg(string ip,int port,string msg);
       
};


#endif