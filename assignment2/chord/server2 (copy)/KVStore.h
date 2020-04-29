#ifndef KVStore_h
#define KVStore_h

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <cstdlib> 
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <aio.h>
#include <bits/stdc++.h>
using namespace std;
extern int set_num;

void write_asynchronously(string key,string value,int set);
string read_sychronously(string key,int set);
void update_synchronously(string key,string value,int set);
int delete_synchronously(string key,int set);
int create_xml();

#endif