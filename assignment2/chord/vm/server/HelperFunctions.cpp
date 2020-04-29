#include"HelperFunctions.h"
#include "KVCache.hpp"
#include "KVStore.h"

int HelperFunctions::find_consitant_hash_id(string str){
	string hextemp, hexVal;
  //  cout<< "strign value :" << str <<endl;
	hextemp=sha1(str);
    hexVal = hextemp.substr(0,4);
    int len = hexVal.length(); 
    int base = 1; 
      
    int dec_val = 0; 
      
    for (int i=len-1; i>=0; i--) 
    {    
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
            base = base * 16; 
        } 
  
        else if (hexVal[i]>='a' && hexVal[i]<='f') 
        { 
            dec_val += (hexVal[i] - 87)*base; 
          
            base = base*16; 
        } 
    } 
  //  cout<< "hash value " <<dec_val <<endl;
    //printf("\n%d",dec_val);
    return dec_val;
}
int HelperFunctions::computeindex(int n,int i){
    int mod_value = pow(2,16);
    int temp=n+pow(2,i-1);
    return temp % mod_value;

}
	string HelperFunctions::combine(char* contact_ip,int port1){
	   string port=to_string(port1);
	   string ip(contact_ip);
	   string hostname;
	   hostname.append(ip);
	   hostname.append(":");
	   hostname.append(port);
	   return hostname;

	      
	}

    int getClosest(int, int, int); 
  

int HelperFunctions::findClosest(int arr[], int n, int target) 
{ 
    
    if (target <= arr[0]) 
        return arr[0]; 
    if (target >= arr[n - 1]) 
        return arr[n - 1]; 

    int i = 0, j = n, mid = 0; 
    while (i < j) { 
        mid = (i + j) / 2; 
  
        if (arr[mid] == target) 
            return arr[mid]; 
  

        if (target < arr[mid]) { 
  
   
            if (mid > 0 && target > arr[mid - 1]) 
                return getClosest(arr[mid - 1], 
                                  arr[mid], target); 
  

            j = mid; 
        } 
  
       
        else { 
            if (mid < n - 1 && target < arr[mid + 1]) 
                return getClosest(arr[mid], 
                                  arr[mid + 1], target); 
         
            i = mid + 1;  
        } 
    } 
 
    return arr[mid]; 
} 
int HelperFunctions::getClosest(int val1, int val2, 
               int target) 
{ 
    if (target - val1 >= val2 - target) 
        return val2; 
    else
        return val1; 
} 
bool HelperFunctions::check_string_for_number(string input){
     for(int i=0;i<input.size();i++){
         if(isdigit(input[i])==0){
              return false;
         }
     }
     
     return true;
}

void HelperFunctions::store_key_value_set(string key_value_set){
     char key_value_chr[key_value_set.size()+1];
     strcpy(key_value_chr,key_value_set.c_str());
     char * c_ptr= key_value_chr;
     string key_value;

     char *tmp;
     string key,value;
     HelperFunctions help;
    
     while(tmp = __strtok_r(c_ptr,"\n",&c_ptr)){
         key_value =tmp;
         int pos = key_value.find_first_of(";");
         key = key_value.substr(0,pos);
         value = key_value.substr(pos+1);
         store_key_value(key,value);
     }
}
int HelperFunctions::get_file_set_Id(string key){
	int sum=0;
	for(int i=0; key[i]!='\0'; i++){
		sum += key[i];
	}
	return sum % 100;
}
void HelperFunctions::store_key_value(string key,string value){
    int fileset = get_file_set_Id(key);
    int status=put(key,value);
	if(status==1)
	{
		update_synchronously(key,value,fileset);
	}
	else
	{
		if(read_sychronously(key,fileset)=="Does not exist"){
			write_asynchronously(key,value,fileset);
		}
		else{
			update_synchronously(key,value,fileset);
		}
		
	}

}

string HelperFunctions::get_key_value_set(){
    string file_temp,line,word;
    fstream fin,fout;
    vector<string> row;
    string key_value_set="";
    for(int i=0;i<100;i++)
    {
        file_temp=to_string(i);
        file_temp.append(".dat");
        fin.open(file_temp,ios::in);
        if(!fin.fail()){
            while(!fin.eof())
            {
    	    row.clear();
    	    getline(fin,line);
            stringstream input(line);
    	    while(getline(input,word,';')){
    		    row.push_back(word);
    	    }
    	    if(row.size()!=0)
            {
	            string key=string(row[0]);
	            string value=string(row[1]);
	            value.erase(remove(value.begin(), value.end(), '\n'), value.end());
	            value.erase(remove(value.begin(), value.end(), '\r'), value.end());
	            if(key.size()!=1)
		        {
		            string temp1=key.substr(0,1);
    		        char first[1];
		            strcpy(first,temp1.c_str());
		            if(first[0]=='\0'){
			            key=key.substr(1);
		            }
		        }   
             key_value_set+=key+";"+value+"\n";  
            }
            }
		}
		fin.close();
	}
    return key_value_set;

}
char* HelperFunctions::read_from_file(char * file_name,int size){
    int fd = open(file_name,O_RDONLY);
    int count=0;
    char *file_content=(char *)malloc(sizeof(char)*size);
    
    if((count=read(fd,file_content,sizeof(char)*size))==-1){
        printf("Error while reading file");
    }
    close(fd);
    file_content[count]='\0';

    return file_content;
}