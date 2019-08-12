#include "fptree.h"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <algorithm>
#define DEBUG
using namespace std;
#define INPUT_FILE "input.txt"
#define OUT_FILE "outfile.txt"
#define SUPPORT_PERCENT "2"

// GLOBAL VARIABLES
int mSupportPercent ; // for storing the support percent
string mOutFile,mInFile; // input and output file
int totalTransactions = 0;
int supportCount = 0; // store support count

// END GLOBALS

/*
	descending sort on the frequency of the items 
*/
bool mCustomSort ( pair<int,int>& A,
					pair<int,int>& B)
{
	if((A.second>B.second)||(A.second==B.second && A.first>B.first))
			return true;
	return false;
}

/*
	delete the Node along with the nodes in the following subtree
*/
void delete_FPTreeNode(FPTreeNode* node)
{
	// if node not NULL
	if(node!=nullptr)
	{
		// delete all the children
		for(auto m:node->children)
		{
			delete_FPTreeNode(m.second);
		}
		delete node;
	}
	return ;
}

/*
	delete the FPtree
*/
void delete_FPTree(FPTree& tree)
{
	// delete the root of the tree
	delete_FPTreeNode(tree.root);
};
/*
	tokenize  the string to set of integers
*/
void pTokenize(const string & A,
			   const char limiter,
			    vector<int>& ans)
{
	// feed the string to the string stream and split				
	stringstream ss(A);
	string temp;
	int temp_int;
	while(getline(ss,temp,limiter)){
		temp_int = stoi(temp);
		ans.push_back(temp_int);
	}
	return ;
}

/*
	tokenize the string to set of integers
*/
void pTokenize(const string & A,
			 const char limiter,
			 unordered_set<int> & ans)
{
	// feed the string to the string stream and split	
	stringstream ss(A);
	string temp;
	int temp_int;
	while(getline(ss,temp,limiter)){
		temp_int = stoi(temp);
		ans.insert(temp_int);
	}
	return ;
}

/*
	To add branch to the tree when we get the set of items
*/
void formTree(FPTreeNode* root,  // root node of the tree
				unordered_set<int>& store, // transaction details
				vector<int>& list, // order of the elements
				vector<FPTreeNode*> &temp, // temp links maker
				vector<Tuple>& mtable, // table of (key,freq and sidelinkptrs)
				int counter,
				int incr=1) // frequency of the set
{

	int size = list.size();
	FPTreeNode* node;
	for(int i=counter+1;i<size;i++){
		if(store.find(list[i])==store.end())
		{
			continue;
		}
		else{
			if(root->children.find(list[i])==root->children.end())
			{
				node = new FPTreeNode(list[i],0,root);
				root->children[list[i]] = node;
			}
			else{
				node = root->children[list[i]];
			}
			store.erase(list[i]);
			node->freq+=incr;
			if(mtable[i].freq==0)
			{
				mtable[i].freq+=incr;
				mtable[i].linkptr = node;
				temp[i] = node;
			}
			else{
				if(node->freq==incr){
					temp[i]->linkPtr = node;
					temp[i] = temp[i]->linkPtr;
				}
				mtable[i].freq++;
			}
			formTree(root->children[list[i]],store,list,temp,mtable,i+1);
			return ;
		}
	}
}
/*
	To make FP tree from the database 
*/
void makeFPTreeFromDB(FPTree& tree,
					vector<int>& list){

	FPTreeNode* root = new FPTreeNode();
	tree.root = root;
	// key map is already set
	Table& table = tree.headTable;

	vector<Tuple> & tup = table.mTable;
	int size = list.size();

	vector<FPTreeNode*> tempLinkStore(size,nullptr);
	for(int i=0;i<size;i++)
	{
		tup.push_back(Tuple(list[i],0));
	}
	vector<FPTreeNode*> tempNodes(list.size(),nullptr);
	ifstream mScanner(mInFile);
	string temp;
	char limiter = ' ';
	while(getline(mScanner,temp)){
		// make a tree here
		unordered_set<int> store;
		pTokenize(temp,limiter,store);
		formTree(root,store,list,tempLinkStore,tup,0);
	}
	return ;
}

void print_Tree(const FPTree& root)
{
	const Table & nTable = root.headTable;
	const vector<Tuple>& toot= nTable.mTable;
	FPTreeNode * temp = toot[toot.size()-1].linkptr;
	FPTreeNode* temp2;
	int incr;
	int count = 0;
	while(temp!=nullptr)
	{
		unordered_set<int> store;
		temp2 = temp;
		incr = temp2->freq;
		cerr<<"# "<<count<<" # ";
		while(temp2->parentptr!=nullptr)
		{
			cerr<<"( "<<temp2->label<<" "<<temp2->freq<<" ) ";
			temp2 = temp2->parentptr;
		}
		cerr<<endl;
		temp = temp->linkPtr;
		count++;
	}
}
/*
	form a conditional tree from given tree
*/
void makeFPTree(const FPTree& root,
				vector<int>& list,
				int label,FPTree &newTree)
{
	FPTree& nTree = newTree;
	FPTreeNode* troot = new FPTreeNode();
	nTree.root = troot;
	Table& nTable = nTree.headTable;
	nTable.keyMap = root.headTable.keyMap;
	const vector<Tuple>& originalTable = root.headTable.mTable;
	vector<Tuple>& mTable = nTable.mTable;

	vector<int> mList;
	int size = list.size();
	int keyIndex = nTable.keyMap[label];
	// made the new list required for the new list
	for(int i=0;i<=keyIndex;i++)
	{
		mList.push_back(list[i]);
		mTable.push_back(Tuple(originalTable[i].key,0));
	}
	// made the table required for the conditional fp tree
	for(int i=0;i<=keyIndex;i++)
	{
		mTable[i].freq = 0;
		mTable[i].linkptr = nullptr;
	}
	
	// links table
	FPTreeNode* temp = mTable[keyIndex].linkptr;
	FPTreeNode* temp2;
	int incr = 0;
	vector<FPTreeNode*> tempNodes(mList.size(),nullptr);
	while(temp!=nullptr)
	{
		unordered_set<int> store;
		temp2 = temp;
		incr = temp2->freq;
		while(temp2->parentptr!=nullptr)
		{
			store.insert(temp2->label);
			temp2 = temp2->parentptr;
		}
		formTree(troot,store,mList,tempNodes,mTable,0,incr);
		temp = temp->linkPtr;
	}
	return ;
}

vector<vector<int>> answer;
void suffixTree(vector<int>& list,vector<int> temp,int count,const FPTree& root)
{

	for(int i = count-1;i>=0;i--)
	{
		if(root.headTable.mTable[i].freq<supportCount)
		{
			continue;
		}
		vector<int> temp2 = temp;
		temp2.push_back(list[i]);
		answer.push_back(temp2);
		FPTree nTree;
		makeFPTree(root,list,list[i],nTree);
		suffixTree(list,temp2,i-1,nTree);
	}
}
int main(int argc, 
		const char *argv[])
{

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
	// scanner for the file
	ifstream mScanner(mInFile);

	string temp;
	vector<int> store;
	unordered_map<int,int> mMap;
	vector<int> tempNumStore;
	char limiter = ' ';
	
	// counting the transactions and storing it in the map
	while(getline(mScanner,temp)){
		store.resize(0);
		pTokenize(temp,limiter,store);
		for(int i=0;i<store.size();i++){
			mMap[store[i]]++;
		}
		totalTransactions++;
	}
	mScanner.close(); // closing the file stream

	// support count
	supportCount = totalTransactions * (mSupportPercent+99)/100;
	
	FPTree tree; // total tree //
	
	Table &tTable = tree.headTable; // temporary table
	unordered_map<int,int> &mKeyMap = tTable.keyMap; // contains KEY || Index
	vector<pair<int,int>> headfreqCount;
	for(auto itr = mMap.begin();itr!=mMap.end();itr++){
		if(itr->second<supportCount){
			mMap.erase(itr);
			continue;
		}
		mKeyMap[itr->first] = 0;
		headfreqCount.push_back(make_pair(itr->first,itr->second));
	}
	sort(headfreqCount.begin(),headfreqCount.end(),mCustomSort);
	vector<int> sorted_list;
	int tempSize = headfreqCount.size();
	for(int i=0;i<tempSize;i++){
		mKeyMap[headfreqCount[i].first] = i;
		sorted_list.push_back(headfreqCount[i].first);
	}
	makeFPTreeFromDB (tree,sorted_list);
	print_Tree(tree);
	// created the tree
	// now conditional fp tree and count marking

	return 0;
}
