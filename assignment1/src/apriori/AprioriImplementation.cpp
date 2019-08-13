#include<bits/stdc++.h>
#include<fstream>
#include <unordered_map> 
using namespace std;

// Global vars
unordered_map<string, int> frequent_itemset_count;
set<string> frequent_itemsets;
int MIN_SUPPORT = 2;

void printFrequentItems(){
    for (auto x : frequent_itemset_count) 
    if(x.second>=MIN_SUPPORT){
      cout << x.first << " => " << x.second << endl; 
    }
}

set<string> parseLine(string line){
	set<string> set;
    char* st = strtok(&line[0]," ");
    while (st != NULL)
    {
        // cout << st << endl;
        set.insert(st);
        st = strtok(NULL," ");
    }
	//  TreeSet<String> sortedSet = new TreeSet<String>(set);  doubt
	return set;
}

bool isSubSequence(string str1, string str2, int m, int n) 
{ 
    // Base Cases 
    if (m == 0)  
        return true; 
    if (n == 0)  
        return false; 
            
    // If last characters of two strings are matching 
    if (str1[m-1] == str2[n-1]) 
        return isSubSequence(str1, str2, m-1, n-1); 

    // If last characters are not matching 
    return isSubSequence(str1, str2, m, n-1); 
} 

void updateFrequentItemSet(string line, int k){
		
    string itemkey; 
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        if (isSubSequence( *it, line,  (*it).length(), line.length())) {	
            if (frequent_itemset_count.find(*it) != frequent_itemset_count.end() ) {
                frequent_itemset_count[*it] = frequent_itemset_count[*it] + 1;	
            } else {
                frequent_itemset_count[*it] = 1;		
            }			  
        }
    }	    	
}

void updateFrequentItemSet(set<string> set){
    string itemkey; 
    for (auto it=set.begin(); it != set.end(); ++it) 
    {
        if ( frequent_itemset_count.find(*it) != frequent_itemset_count.end() ) {
            frequent_itemset_count[*it] = frequent_itemset_count[*it] + 1;	
        } else {
            frequent_itemset_count[*it] = 1;		
        }
    }
}

void IterateFileLinebyLine(string path, int k){
		
    ifstream read;
    set<string> set;
    string line;
    read.open(path);
    if(read.is_open()){        
        while(getline (read,line)){
            /*
            * CALL THE PARSER TO PARSE THE LINE
            */
            if (k == 1){
                set = parseLine(line);
                //System.out.println("Line Read is following" + line);
                //System.out.println("Set of element is following" + set);
                updateFrequentItemSet(set);
            }else {
                /*
                * Take each line, 
                * Check whether the pruneditemwiselist item matches with substring
                * but for this, first sort the substring i.e. line of the file
                * if yes then update the count in the GlobalVars.frequent_itemset_count
                */
                updateFrequentItemSet( line, k);
            }
        }
        read.close();
    } else{
        cout<< "input file error"<<endl;
    }   
}

set<string> pruning(){ 
    set<string> pruneditemkeylist;
    for (auto it=frequent_itemsets.begin(); it != frequent_itemsets.end(); ++it) 
    {
        if (frequent_itemset_count.find(*it)!= frequent_itemset_count.end() 
                && frequent_itemset_count[*it] >= MIN_SUPPORT){
            pruneditemkeylist.insert(*it);
        }
    } 
    return pruneditemkeylist;
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
    string tempitem;
    
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
                    tempitem = s1.substr(s1.find_last_of(" ") + 1);
                    if ( s.find(tempitem) == string::npos){
                        string temp = s + " " + tempitem;
                        sort(temp.begin(), temp.end(),customsort); 
                        if (tempitemkeylist.find(temp) == tempitemkeylist.end()){
                            // cout << temp << endl;
                            tempitemkeylist.insert(temp);
                        }
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

            set<string> temp;
            for (auto i : frequent_itemset_count) {
                temp.insert(i.first);
            }
            frequent_itemsets = temp;
            // frequent_itemsets = frequent_itemset_count.keySet();  doubt
            k++;
        } else {
            frequent_itemsets = pruning();
            frequent_itemsets = generatenextlevelCandidate(k);
            IterateFileLinebyLine(path, k); // change the update itemwise count 
            k++;
        }
    } while (frequent_itemsets.size() > 0);
	cout<< "path: "<<path<< endl;

    return theMap;
}


int main(int argc,const char* argv[])
{
    string in_file;
    if (argc==2)
    {
       in_file = argv[1];
    }
    else{
        cerr<<"Insufficient or More arguments\n";
    }
    parseFile(in_file);
    printFrequentItems();

    return 0;
}