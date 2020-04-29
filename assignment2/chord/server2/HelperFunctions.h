
#ifndef HelperFunctions_h
#define HelperFunctions_h
#include <iostream>
#include "sha1.h"
#include<string>
#include<math.h>
#include <netdb.h>
#include<string.h>
#include<strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include"KVClientHandler.h"
#include "KVCache.hpp"
#include "KVStore.h"
using namespace std;


class HelperFunctions{
   public:
       int find_consitant_hash_id(string str);
       int computeindex(int n,int i);
       string combine(char* contact_ip,int port1);
        int getClosest(int, int, int); 
        int findClosest(int arr[], int n, int target);
        bool check_string_for_number(string);
        void store_key_value_set(string);
         void store_key_value(string,string);
         int get_file_set_Id(string key);
         string get_key_value_set();
      
            



};
#endif