#include<iostream>
#include<stdio.h>
#include<string>

using namespace std;
bool check_string_for_number(string input){
     for(int i=0;i<=input.size();i++){
         if(isdigit(input[i])==0){
              return false;
         }
     }
     return true;
}
int main(){
    string input = "1234";
   
    cout << check_string_for_number(input);
    return 0;
}