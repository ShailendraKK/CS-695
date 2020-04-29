#ifndef _KVClientLibrary_H_
#define _KVClientLibrary_H_
#include <string>
using namespace std;

string convert_to_xml(char command[],char key[],char value[]);
void parse_xml(string &Message,string &key_par,string &value_par,char *input_par);

#endif