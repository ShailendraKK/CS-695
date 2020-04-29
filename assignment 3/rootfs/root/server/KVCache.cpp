
#include "KVCache.hpp"

struct keyValue{
	string Key;
	string Value;
	int secondChance = 0;
	int term=0;
};
std::vector<keyValue> KV;
int cacheSize, numSet;
int put(string key,string value){
	int set = getSetId(key);
	int index = searchKey(key, set);
	/*if(KV[0].Key.compare("\0")==0){
		cout<< "NULL"<< endl;
	}
	else{
		cout<<"NOT NULL"<<endl;
	}*/
	int size = cacheSize / numSet, last = (size + (set * size)), status=0;
	if(index==-1){
		while(status == 0){
			for(int i = set * size; i < last && status == 0; i++){
				if (KV[i].term == 1 && KV[i].secondChance==0){
					if(KV[i].Key.compare("\0")==0){  // If Empty space
						KV[i].Key=key;
						KV[i].Value=value;
						KV[i].term=0;
						KV[i].secondChance=0;
						if(i==last-1){
							for(int j= set * size; j < last; j++){
								KV[j].secondChance=0;
							}
							KV[set * size].term=1;
						}
						else{
							KV[i+1].term=1;
						}
						status = 1;
					}
					else{
						if(KV[i].secondChance==0){ // Replace with present value
							KV[i].Key=key;
							KV[i].Value=value;
							KV[i].term=0;
							KV[i].secondChance=0;
							if(i==last-1){
								for(int j= set * size; j < last; j++){
									KV[j].secondChance=0;
								}
								KV[set * size].term=1;
							}
							else{
								KV[i+1].term=1;
							}
							status = 1;
						}
					}
				}
				else if(KV[i].term==1 && KV[i].secondChance==1){
					KV[i].term=0;

					if(i==last-1){
						for(int j= set * size; j < last; j++){
							KV[j].secondChance=0;
						}
						KV[set * size].term=1;
					}
					else{
						KV[i+1].term=1;
					}
				}
			}
		}
		/*
		int size = KV.size();
		KV[size - 1].Key=key;
		KV[size - 1].Value=value;
		KV[size - 1].secondChance=0;
		*/
	}
	else{
		KV[index].Value=value;
		KV[index].secondChance=1;
		return 1;  //update
	}
	return 0;
}

string get(string key){
	int set = getSetId(key);
	int index = searchKey(key, set);
	if(index == -1){
		return "miss";
	}
	KV[index].secondChance=1;
	//cout << key << " : " << KV[index].Value << endl;
	return KV[index].Value;
}

int remove(string key){
	int set = getSetId(key);
	int i = searchKey(key, set);
	if(i == -1){
		return -1;
	}
	KV[i].Key.erase();
	KV[i].Value.erase();
	return 0;
}

int searchKey(string key, int set){
	int status = 0;
	int size = cacheSize / numSet, last = (size + (set * size));
	for (int i = set * size; i < last; i++)
	{
		if(key.compare(KV[i].Key)==0){
			return i;
			break;
		}
	}
	return -1;
}

int getSetId(string key){
	int sum=0;
	for(int i=0; key[i]!='\0'; i++){
		sum += key[i];
	}
	return sum % numSet;
}

int init(int set_entries,int sets){
	numSet=sets;
	cacheSize=set_entries*sets;
	for(int i=0; i<cacheSize; i++){
		KV.push_back(keyValue());
		if(i % set_entries == 0){
			KV[i].term=1;
		}
	}
	return 0;
}
void print(){
		 cout << "**********************************************\n";
				int size = KV.size(), s=0;
			for (int i = 0; i < size; i++){ 
				if(i % (cacheSize / numSet)==0){
					cout << "set number: " << s++ <<endl;
				}
	        	cout << KV[i].Key << ":" << KV[i].Value << endl;
	        } 
	        cout << "**********************************************\n";
}
/*
int main(){
	int choice,numEntries;
	cout << "Enter no of entries: ";
	cin >> numEntries;
	cout << "Enter Number of sets: ";
	cin >> numSet;
	cacheSize=numEntries*numSet;
	init(numEntries,numSet);

	while (1){
		cout << "Enter 1 for put.\n" << "Enter 2 for get.\n" << "Enter 3 for remove.\n" << "Enter 4 to print vector\n" << "Enter 5 to exit.\n";
		cin >> choice;
		if(choice == 1){
			cout << "Enter key: ";
			cin >> key;
			cout << "Enter value: ";
			cin >> value;
			put(key);
		}
		else if(choice == 2){
			cout << "Enter Key: ";
			cin >> key;
			int o = get(key);
			if (o == -1)
			{
				cout << "MISS.\n";
			}
			else{
				cout << key << " : " << KV[o].Value << endl;
			}
		}
		else if(choice == 3){
			cout << "Enter Key: ";
			cin >> key;
			int o = remove(key);
			if(o == -1){
				cout << "Key Not Present\n";
			}
		}
		else if(choice == 4){
			int size = KV.size(), s=0;
			for (int i = 0; i < size; i++){ 
				if(i % (cacheSize / numSet)==0){
					cout << "set number: " << s++ <<endl;
				}
	        	cout << KV[i].Key << ":" << KV[i].Value << endl;
	        } 
		}
		else if(choice == 5){
			break;
		}
		cout << "**********************************************\n";
	}
}*/