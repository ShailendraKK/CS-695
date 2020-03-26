#include "KVClientHandler.h"
#include <iostream>
#include <string>
#include <bits/stdc++.h>
using namespace std;


void parse_xml(string &command_par,string &key_par,string &value_par,char *input_par){
		string genxml_1="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"getreq\">\n<Key>keyrep</Key>\n</KVMessage>";
	string genxml_2="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVMessage type=\"putreq\">\n<Key>keyrep</Key>\n<Value>valrep</Value>\n</KVMessage>";
		string input_cpy1=string(input_par);
	string input_cpy2=input_cpy1;
	string input_cpy3=input_cpy2;

	regex key_re("<Key>([^<]+)<[/]Key>");
	regex value_re("<Value>([^<]+)<[/]Value>");
	regex command_re("<KVMessage type=([^>]+)>");
	smatch key_match,value_match,command_match;
	if(regex_search(input_cpy1,key_match,key_re)==true)
	{
			key_par=key_match.str(1);

	}
	if(regex_search(input_cpy2,value_match,value_re)==true){
		value_par=value_match.str(1);


	}
	if(regex_search(input_cpy3,command_match,command_re)==true){
			command_par=command_match.str(1);
	
	}


}
