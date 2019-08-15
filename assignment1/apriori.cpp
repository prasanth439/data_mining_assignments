#include <sstream>
#include <iostream>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstring>

using namespace std;

#define TIMEOUT_TIME 1680
#define FLUSH_FREQ 10000

// Global vars
unordered_map<string, int> frequent_itemset_count;
set<string> frequent_itemsets;
int MIN_SUPPORT = 1;
double support_thershold;


int mSupportPercent ; // for storing the support percent
string mOutFile; // input and output file
int totalTransactions = 0;
int supportCount = 0; // store support count
time_t start_time,end_time;
ofstream ofle; // open file 
// int map_count = 0;
int mode;


bool check_time()
{
	time(&end_time);
	if ((end_time-start_time) >TIMEOUT_TIME)
	{
		return true;
	}
	return false;
}

void flush_freq(int mode){
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it){
      if(mode){
        ofle << *it << "\n";
      }  
      frequent_itemset_count.erase(*it);
    } 
}
void printFrequentItems(){
    for (auto x : frequent_itemset_count) 
      cout << x.first << " => " << x.second << endl; 
}

void parseLine(string line,set<string>&s){
    set<string> set;
    char* st = strtok(&line[0]," ");
    while (st != NULL)
    {
        set.insert(st);
        st = strtok(NULL," ");
    }
    s = set;
}
void tokenizeLine(string line,vector<string> &tokline){
    char* st = strtok(&line[0]," ");
    while (st != NULL)
    {
        if(frequent_itemset_count.find(st)!=frequent_itemset_count.end()){
            tokline.push_back(st);
        }
        st = strtok(NULL," ");
    }

}
void tokenizeLineandUpdate(string line){
    char* st = strtok(&line[0]," ");
    while (st != NULL)
    {
        if ( frequent_itemset_count.find(st) != frequent_itemset_count.end() ) {
            frequent_itemset_count[st] = frequent_itemset_count[st] + 1;	
        } else {
            frequent_itemset_count[st] = 1;
            // map_count ++;
            frequent_itemsets.insert(st); // for the first time.... one time occuring 
        }
        st = strtok(NULL," ");
    }

}

bool isExisting(vector<string> str1,vector<string> tokline) 
{ 
    for(auto s1: str1){
        bool exist = false;
        for(auto s2:tokline){
            if(s1.compare(s2)==0){
                exist = true;
                break;
            }
        }
        if(exist==false){
            return false;
        }    
    }
    return true;
} 

void updateFrequentItemSet(string line, int k){
		
    string itemkey; 
    vector<string> tokline;
    tokenizeLine(line,tokline);

    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        vector<string> tokstr1;
        tokenizeLine(*it,tokstr1);
        if (isExisting( tokstr1, tokline)) {	
            if (frequent_itemset_count.find(*it) != frequent_itemset_count.end() ) {
                frequent_itemset_count[*it] = frequent_itemset_count[*it] + 1;	
            } else {
                frequent_itemset_count[*it] = 1;	
                // map_count++;	
            }			  
        }
    }	    	
}

// void updateFrequentItemSet(vector<string> &set){
//     string itemkey; 
//     for (auto it=set.begin(); it != set.end(); ++it) 
//     {
//         if ( frequent_itemset_count.find(*it) != frequent_itemset_count.end() ) {
//             frequent_itemset_count[*it] = frequent_itemset_count[*it] + 1;	
//         } else {
//             frequent_itemset_count[*it] = 1;
//             frequent_itemsets.insert(*it);
//         }
//     }
// }

void IterateFileLinebyLine(string path, int k){
		
    ifstream read;
    set<string> set;
    string line;
    read.open(path);
    if(read.is_open()){        
        while(getline (read,line)){
            if (k == 1){
                vector<string> tokenline;
                // parseLine(line,set);
                // tokenizeLine(line,tokenline);
                // updateFrequentItemSet(tokenline);
                tokenizeLineandUpdate(line);
                totalTransactions++;
                
            }else {
                updateFrequentItemSet( line, k);
            }
        }
        read.close();
    } else{
        cout<< "input file error"<<endl;
    }   
}

void pruning(set<string> &freq ){ 
    set<string>pruneditemkeylist;
    // need to prune freq_item_count searching uneccesary maps
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        if (frequent_itemset_count.find(*it)!= frequent_itemset_count.end()){
            if(frequent_itemset_count[*it] >= MIN_SUPPORT){
                pruneditemkeylist.insert(*it);
            }else{
                frequent_itemset_count.erase(*it);
            }
        }
    }
    freq = pruneditemkeylist; 
}

bool optimizeApriori(string s, string s1, int k){
      
    if (k == 2){
        return true;
    }
    string s_temp = s.substr(0, s.find_last_of(" "));
    string s1_temp = s1.substr(0, s1.find_last_of(" "));
    if (s_temp.compare(s1_temp)==0){
        return true;
    } else {
        return false;
    }
}

bool customsort(char a,char b){
    if(a==' '|| b ==' '){
        return false;
    }
    else{
        return a<b;
    }
}
set<string> generatenextlevelCandidate(int k){
		
    vector<string> list(frequent_itemsets.begin(), frequent_itemsets.end());
    set<string> tempitemkeylist;
    string tempitem1,tempitem;
    
    string s, s1;
    if(list.size()==0){
        return tempitemkeylist;
    }
    else{
        for (int i = 0; i < list.size()-1 ; i++){
            s = list[i];
            
            for (int j = i + 1; j < list.size() ; j++){
                s1 = list[j];
                if (optimizeApriori(s, s1, k)){
                    tempitem1 = s1.substr(s1.find_last_of(" ") + 1);
                    tempitem = s.substr(s.find_last_of(" ") + 1);
                    string temp ;
                    if(tempitem < tempitem1){
                        temp = s + " " + tempitem1;
                    }
                    else{
                        temp = s1 + " " + tempitem;
                    }
                    if (tempitemkeylist.find(temp) == tempitemkeylist.end()){
                        tempitemkeylist.insert(temp);
                    }
                }
            }
        }
        return tempitemkeylist;
    }
}

void parseFile(string path)
{
    int k =1;
    do {
        if (k == 1){

            IterateFileLinebyLine(path, k);
            MIN_SUPPORT = (totalTransactions*mSupportPercent+99)/100;
            pruning(frequent_itemsets);
            flush_freq(mode);
            k++;

        } else {
            if(check_time())
            {
                if(!mode){
                    ofle<<end_time-start_time<<"\n";
                }
                ofle.close();
                exit(0);
            }
            frequent_itemsets = generatenextlevelCandidate(k);
            IterateFileLinebyLine(path, k); // change the update itemwise count 
            pruning(frequent_itemsets);
            flush_freq(mode);
            // customprint(k);
            k++;
        }
    } while (frequent_itemsets.size() > 0);
	// cout<< "path: "<<path<< " "<<k << endl;
}

int main(int argc,const char* argv[])
{
    time(&start_time);
    string in_file,out_file;

// ./$apriEXE $number $dataset $filename
    if (argc==5)
    {
        mSupportPercent = atof(argv[1]);
        // MIN_SUPPORT = atof(argv[1]);
        in_file = argv[2];
        mOutFile = argv[3];
        mode = stoi(argv[4]);
        ofle.open(mOutFile,ios::app);
    }
    else{
        cerr<<"Insufficient or More arguments\n";
    }
    parseFile(in_file);
    // printFrequentItems();

    // flush_data(mode);
	if(mode){
		
	}
	else{
		ofle<<end_time-start_time<<"\n";
		ofle.close();
	}

    return 0;
}
