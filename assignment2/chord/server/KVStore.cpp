#include "KVStore.h"
void write_asynchronously(string key,string value,int set){
	string write_txt=key;
	//char line_sep[]="\u2026";
	write_txt.append(";");
	write_txt.append(value);
	write_txt.append("\n");
		string filename=to_string(set);
	filename.append(".dat");
	char file_name[filename.length()];
	strcpy(file_name,filename.c_str());
	int size=write_txt.length();
	char file_content[size];
	struct aiocb async_write;
	struct aiocb *async_write_p[1];
	strcpy(file_content,write_txt.c_str());
	char *file_content_ptr=file_content;

	  int outfile=open(file_name, O_CREAT|O_RDWR, 0777);
  close(outfile);
 int write_fd=open(file_name, O_APPEND|O_RDWR, 0777);
	//int write_fd=open(file_name,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
	if(write_fd==-1){
	//	printf("Error while opening file");
		return;
	}

	memset(&async_write,0,sizeof(async_write));
	async_write.aio_fildes=write_fd;
	async_write.aio_buf=file_content_ptr;
	async_write.aio_nbytes=size;

	if(aio_write(&async_write)==-1){
		//printf("Error while writing to file");
		close(write_fd);
		return;
	}
	  
   
  //while ((err = aio_error (&async_write)) == EINPROGRESS);
  async_write_p[0]=&async_write;
  aio_suspend(async_write_p,1,NULL);
  int error = aio_error(&async_write);

  if (error != 0) {
  //  printf("aio_error\n");
    close(write_fd);
   	return;
  }
 
  int return_value= aio_return(&async_write);

  if (return_value != size) {
   // printf("Error while returning from kernal to application\n");
    close(write_fd);
    return;
  }
	close(write_fd); 

}
string read_sychronously(string key,int set){
	fstream fileStream;
	string filename=to_string(set);
	filename.append(".dat");

    fileStream.open(filename);
    if(fileStream.fail()){
    	return "Does not exist";
    }

	fstream fin;
	int flag,flag2=0;
	fin.open(filename,ios::in);
	
	vector<string> row;

	string line,word,temp;
	//char delim='\u2026';
		//char delim='\u2026';
	while(!fin.eof()){
		row.clear();
		
		getline(fin,line);
		stringstream input(line);
		while(getline(input,word,';')){
			row.push_back(word);
		}
		if(row.size()==0){

					fin.close();
					return "Does not exist";

		}
		string in_key=string(row[0]);
		if(in_key.size()!=1)
		{
		string temp1=in_key.substr(0,1);

		char first[1];
		strcpy(first,temp1.c_str());
		if(first[0]=='\0'){
			in_key=in_key.substr(1);
		}
		}
		if(in_key==key){
			//cout << string(row[1]);
			fin.close();
			return string(row[1]);
		}
		flag=1;
	}
	
		fin.close();
		return "Does not exist";
	
}

void update_synchronously(string key,string value,int set){
string write_txt=key;
fstream fin,fout;
int flag1,flag=0;
string filename=to_string(set);
filename.append(".dat");
fin.open(filename,ios::in);
string bck_file=filename;
bck_file.append("_bck");
fout.open(bck_file,ios::out);
vector<string> row;
string line,word;
while(!fin.eof()){
	row.clear();
	getline(fin,line);
	
	stringstream input(line);
	while(getline(input,word,';')){
		row.push_back(word);
	}

	string in_key=string(row[0]);
	if(in_key.size()!=1)
		{
		string temp1=in_key.substr(0,1);

		char first[1];
		strcpy(first,temp1.c_str());
		if(first[0]=='\0'){
			in_key=in_key.substr(1);
		}
		}
	if(in_key==key){
		flag1=1;
		 row[1]=value;
		 if(!fin.eof()){
		 	fout<<string(row[0])<<";"<<string(row[1])<<"\n";
		 }
	}
	else{
		if(!fin.eof()){
			
		 if(in_key==key){
		
		 row[1]=value;
		 if(!fin.eof()){
		 	fout<<string(row[0])<<";"<<string(row[1])<<"\n";
		  }
	     }
		 	else{
		 	fout<<string(row[0])<<";"<<string(row[1])<<"\n";
		 }
		}
	}
	flag=1;
	if(fin.eof())
		break;
}
 fin.close();
 fout.close();
 remove(filename.c_str());
 rename(bck_file.c_str(),filename.c_str());

}
int delete_synchronously(string key,int set){
string write_txt=key;
fstream fin,fout;
int flag1,flag=0;
string filename=to_string(set);
filename.append(".dat");
fin.open(filename,ios::in);
string bck_file=filename;
bck_file.append("_bck");
fout.open(bck_file,ios::out);

vector<string> row;
string line,word;
while(!fin.eof()){
	row.clear();
	getline(fin,line);
	stringstream input(line);
	while(getline(input,word,';')){
		row.push_back(word);
	}
	if(row.size()==0){
		 fin.close();
 		fout.close();
 		 remove(filename.c_str());
 rename(bck_file.c_str(),filename.c_str());
 		return 0;
	}
	string in_key=string(row[0]);
	if(in_key.size()!=1)
		{
		string temp1=in_key.substr(0,1);

		char first[1];
		strcpy(first,temp1.c_str());
		if(first[0]=='\0'){
			in_key=in_key.substr(1);
		}
	}
	
	if(in_key!=key){
		if(!fin.eof()){
				 	
		 	fout<<string(row[0])<<";"<<string(row[1])<<"\n";
		 
		}
	}
	flag=1;
	if(fin.eof())
		break;
}
 fin.close();
 fout.close();

 remove(filename.c_str());
 rename(bck_file.c_str(),filename.c_str());
return 0;
}
int create_xml(){
	string write_txt=string();
	fstream fileStream;
	string filename="Data.xml"; 
	char file_name[filename.length()];
	strcpy(file_name,filename.c_str());
  	struct stat stats_buff;

  /*  fileStream.open(filename);
    if(fileStream.fail()){
    	    	
    }
    else{
    	write_txt.append("<KVPair>\n");
    }*/
  	fstream fin,fout;
  		vector<string> row;
  		fout.open(filename,ios::out);
	string line,word,file_temp;
    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
    fout << "<KVStore>"<<endl;
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
    fout << "<KVPair>"<<endl;
    fout << "<Key>";
    fout << key;
    fout << "</Key>"<<endl;
	fout << "<Value>";
	fout << value;
	fout << "</Value>"<<endl;
	fout << "</KVPair>"<<endl;
}
}
		}
		fin.close();
	}

	fout << "</KVStore>" << endl;
	fout.close();
	
/*
	int size=write_txt.length();
	char file_content[size];
	strcpy(file_content,write_txt.c_str());
	char *file_content_ptr=file_content;
	struct aiocb async_write;
	struct aiocb *async_write_p[1];
	


 int write_fd=open(file_name, O_CREAT|O_RDWR, 0777);
	if(write_fd==-1){
		printf("Error while opening file\n");
		return -1;
	}

	
	memset(&async_write,0,sizeof(async_write));
	async_write.aio_fildes=write_fd;
	async_write.aio_buf=file_content_ptr;
	async_write.aio_nbytes=size;
	if(aio_write(&async_write)==-1){

		close(write_fd);
		return -1;
	}
	  
   
   async_write_p[0]=&async_write;
  aio_suspend(async_write_p,1,NULL);
  int error = aio_error(&async_write);

  if (error != 0) {
  //  printf("aio_error\n");
    close(write_fd);
   	return -1;
  }
 
  int return_value= aio_return(&async_write);

  if (return_value != size) {
  //  printf("IO Error\n");
    close(write_fd);
    return -1;
  }
	close(write_fd); 
*/
	return 0;
	
}

