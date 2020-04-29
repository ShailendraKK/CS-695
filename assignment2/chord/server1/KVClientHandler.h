#ifndef _KVClientHandler_H_
#define _KVClientHandler_H_
#include <string>
using namespace std;

string convert_to_xml(char command[],char key[],char value[]);
void parse_xml(string&,string&,string&,char *);

#endif