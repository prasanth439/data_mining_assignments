#include<bits/stdc++.h>

#define DEBUG
using namespace std;
#define INPUT_FILE "input.txt"
#define OUT_FILE "outfile.txt"
#define SUPPORT_PERCENT "2"

// GLOBAL VARIABLES
int mSupportPercent ; // for storing the 
string mOutFile,mInFile;
// END GLOBALS

class FPTreeNode {
        public:
                int freq;
                vector<FPTreeNode*> children;
                FPTreeNode *parentptr;
                FPTreeNode *linkPtr;
        public:
                FPTreeNode():parentptr(nullptr)
                             ,linkPtr(nullptr)
                             ,freq(-1)
                {}
};

class Tuple{
        public:
                int key;
                int freq;
                FPTreeNode* linkptr;
        public:
                Tuple(int k,int f):linkptr(nullptr){
                        key = k;
                        freq = f;
                }
};

bool mCustomSort(Tuple& A,Tuple& B){
        if((A.freq>B.freq)||(A.freq==B.freq && A.key>B.key)
                        return true;
        return false;
}

void pTokenize(const string & A, const char limiter, vector<int>& ans){
        stringstream ss(A);

        int temp;
        while(getline(ss,temp,limiter)){
                ans.push_back(temp);
        }
        return ;
}


void makeFPTree(vector<Tuple>& index,unordered_map<int,int>& mMap){

        ifstream mScanner(mInFile);

        string temp;
        while(getline(mScanner,temp)){
                // make a tree here
        }

}
int main(int argc, const char *argv[]){

        // for transactions
        // first scan of database
        // store support percentage
#ifndef DEBUG
        if(argc<4){
                cerr<<__LINE__+" "<<"insufficient arguements\n";
                exit(0);
        }
        mSupportPercent = stoi(argv[1]);
        mInFile = argv[2];
        mOutFile = argv[3];
#else
        mSupportPercent = stoi(SUPPORT_PERCENT);
        mInFile = INPUT_FILE;
        mOutFile = OUT_FILE;
#endif
        ifstream mScanner(mInFile);

        string temp;
        vector<int> store;
        unordered_map<int,int> mMap;
        vector<int> tempNumStore;
        char limiter = ' ';
        int totalTransactions = 0;
        int supportCount = 0;
        while(getline(mScanner,temp)){
                store.resize(0);
                pTokenize(temp,limiter,store);
                for(int i=0;i<store.size();i++){
                        mMap[store[i]]++;
                }
                totalTransactions++;
        }
        mScanner.close(); // closing the file stream

        supportCount = totalTransactions * (mSupportPercent+99)/100;

        vector<Tuple> headTable;
        unordered_map<int,int> mKeyMap;

        for(auto itr = mMap.begin();i!=mMap.end();itr++){
                if(itr.second<supportCount){
                        mMap.erase(itr);
                }
                mKeyMap[itr.first] = 0;
                headTable.push_back(Tuple(itr.key,itr.freq));
        }

        sort(headTable.begin(),headTable.end(),mCustomSort);
        int tempSize = headTable.size();
        for(int i=0;i<tempSize;i++){
                mKeyMap[headTable[i].key] = i;
        }

        // make the FPTreeNodes
        makeFPTree (headTable,mKeyMap);


        return 0;
}

