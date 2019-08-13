#include<bits/stdc++.h>
#include<fstream>
#include <unordered_map> 
using namespace std;

// Global vars
unordered_map<string, int> frequent_itemset_count;
set<string> frequent_itemsets;
int MIN_SUPPORT = 1;
double support_thershold;

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
        tokline.push_back(st);
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
            frequent_itemsets.insert(st);
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
            }else {
                updateFrequentItemSet( line, k);
            }
        }
        read.close();
    } else{
        cout<< "input file error"<<endl;
    }   
}
void pruningFreqCount(set<string> &freq ){ 
    set<string>pruneditemkeylist;
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        if (frequent_itemset_count.find(*it)!= frequent_itemset_count.end() 
                && frequent_itemset_count[*it] >= MIN_SUPPORT){
            pruneditemkeylist.insert(*it);
        }
    }
    freq = pruneditemkeylist; 
}
void pruning(set<string> &freq ){ 
    set<string>pruneditemkeylist;
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        if (frequent_itemset_count.find(*it)!= frequent_itemset_count.end() 
                && frequent_itemset_count[*it] >= MIN_SUPPORT){
            pruneditemkeylist.insert(*it);
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

map<string, int> parseFile(string path)
{
    map<string, int> theMap;
    int k =1;
    do {
        if (k == 1){
            IterateFileLinebyLine(path, k);
            pruningFreqCount(frequent_itemsets);
            k++;
        } else {
            frequent_itemsets = generatenextlevelCandidate(k);
            IterateFileLinebyLine(path, k); // change the update itemwise count 
            pruning(frequent_itemsets);
            k++;
        }
    } while (frequent_itemsets.size() > 0);
	cout<< "path: "<<path<< " "<<k << endl;

    return theMap;
}

int main(int argc,const char* argv[])
{
    string in_file,out_file;
// ./$apriEXE $number $dataset $filename
    if (argc==4)
    {
       MIN_SUPPORT = atof(argv[1]);
       in_file = argv[2];
       out_file = argv[3];
    }
    else{
        cerr<<"Insufficient or More arguments\n";
    }
    parseFile(in_file);
    printFrequentItems();

    return 0;
}
