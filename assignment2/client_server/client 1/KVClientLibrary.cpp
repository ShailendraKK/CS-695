#include "KVClientLibrary.h"
#include <iostream>
#include <string>
#include <bits/stdc++.h>
using namespace std;


string convert_to_xml(char command_par[],char key_par[],char value_par[]){
	string genxml_1="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"getreq\">\n<Key>keyrep</Key>\n</KVMessage>";
	string genxml_2="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"putreq\">\n<Key>keyrep</Key>\n<Value>valrep</Value>\n</KVMessage>";

	string command(command_par);
	string key(key_par);
	string value(value_par);
	string modified_xml="hi there";
	if(command.compare("get")==0)
	{	
		
		genxml_1.replace(70,6,key);
		return genxml_1;
		
	}
	else if (command.compare("put")==0)
	{
	
		genxml_2.replace(90,6,value);
		genxml_2.replace(70,6,key);
		
		return genxml_2;
		
	}
	else if (command.compare("del")==0)
	{
	

		genxml_1.replace(70,6,key);
		genxml_1.replace(56,6,"delreq");
		return genxml_1;
		
	}

	
	
  	return modified_xml;
}
void parse_xml(string &Message,string &key_par,string &value_par,char *input_par){
		string genxml_1="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"resp\">\n<Key>keyrep</Key>\n<Value>valrep</Value>\n</KVMessage>";
	string genxml_2="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"resp\">\n<Message>Success</Message>\n</KVMessage>";
	string input_cpy1=string(input_par);
	string input_cpy2=input_cpy1;
	string input_cpy3=input_cpy2;

	regex key_re("<Key>([^<]+)<[/]Key>");
	regex value_re("<Value>([^<]+)<[/]Value>");
	regex message_re("<Message>([^<]+)<[/]Message>");
	smatch key_match,value_match,command_match;
	if(regex_search(input_cpy1,key_match,key_re)==true)
	{
		key_par=key_match.str(1);
		}
	if(regex_search(input_cpy2,value_match,value_re)==true){
		value_par=value_match.str(1);
	
	}
	if(regex_search(input_cpy3,command_match,message_re)==true){
		Message=command_match.str(1);

	}


}
