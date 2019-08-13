#include "fptree.h"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <algorithm>

#define DEBUG

using namespace std;

#define INPUT_FILE "input.txt"
#define OUT_FILE "outfile.txt"
#define SUPPORT_PERCENT "10"


// GLOBAL VARIABLES
int mSupportPercent ; // for storing the support percent
string mOutFile,mInFile; // input and output file
int totalTransactions = 0;
int supportCount = 0; // store support count

vector<pair<vector<int>,int>> freq_sets;
unordered_map<int,int> GCompare;
// END GLOBALS

/*
	descending sort on the frequency of the items 
*/
struct set_comparator {
    bool operator() (int a,int  b) const {
        return ((GCompare[a]<GCompare[b])||((GCompare[a]==GCompare[b])&&(a<b)));
    }
};
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
void formTree(FPTreeNode* ARoot,  // root node of the tree
				set<int,set_comparator>& branchNodes, // transaction details
				vector<FPTreeNode*> &ATempLinks, // temporary links maker
				vector<Tuple>& ATableTups, // table of (key,freq and sidelinkptrs)
				int incr=1) // frequency of the set
{

	FPTreeNode* node;
	for(auto itr:branchNodes){
		if(ARoot->children.find(itr)==ARoot->children.end())
		{
			node = new FPTreeNode(itr,0,ARoot);
			ARoot->children[itr] = node;
		}
		else{
			node = ARoot->children[itr];
		}
		node->freq+=incr;
		int i = GCompare[itr];// Get the index from table
		if(ATableTups[i].freq==0)
		{
			ATableTups[i].freq+=incr;
			ATableTups[i].linkptr = node;
			ATempLinks[i] = node;
		}
		else{
			if(node->freq==incr){
				ATempLinks[i]->linkPtr = node;
				ATempLinks[i] = ATempLinks[i]->linkPtr;
			}
			ATableTups[i].freq+=incr;
		}
		ARoot = node;
	}
}

void formTree(FPTreeNode* ARoot,  // root node of the tree
				vector<int>& branchNodes, // transaction details
				vector<FPTreeNode*> &ATempLinks, // temporary links maker
				vector<Tuple>& ATableTups, // table of (key,freq and sidelinkptrs)
				int incr=1) // frequency of the set
{


	FPTreeNode* node;
	int itr;
	for(int k = branchNodes.size()-1;k>=0;k--){
		itr = branchNodes[k];
		if(ARoot->children.find(itr)==ARoot->children.end())
		{
			node = new FPTreeNode(itr,0,ARoot);
			ARoot->children[itr] = node;
		}
		else{
			node = ARoot->children[itr];
		}
		node->freq+=incr;
		int i = GCompare[itr];// Get the index from table
		if(ATableTups[i].freq==0)
		{
			ATableTups[i].freq+=incr;
			ATableTups[i].linkptr = node;
			ATempLinks[i] = node;
		}
		else{
			if(node->freq==incr){
				ATempLinks[i]->linkPtr = node;
				ATempLinks[i] = ATempLinks[i]->linkPtr;
			}
			ATableTups[i].freq+=incr;
		}
		ARoot = node;
	}
}

/*
	To make FP tree from the database 
*/
void makeFPTreeFromDB(FPTree& A,
					vector<int>& AList){

	// Create root and set it to A
	FPTreeNode* ARoot = new FPTreeNode();
	A.root = ARoot;
	// key map is already set
	Table& ATable = A.headTable;
	vector<Tuple> & ATableTups = ATable.mTable;
	int AListSize = AList.size();
	vector<FPTreeNode*> tempLinkStore(AListSize,nullptr);
	for(int i=0;i<AListSize;i++)
	{
		ATableTups.push_back(Tuple(AList[i],0));
	}
	vector<FPTreeNode*> ATempLinks(AList.size(),nullptr);
	ifstream fileScanner(mInFile);
	string temp;
	char limiter = ' ';
#ifdef DEBUG
	int count = 0;
	cerr<<"# Second Scan of DB\n";
#endif
	while(getline(fileScanner,temp)){
		// make a tree here
		vector<int> branchNodes;
		pTokenize(temp,limiter,branchNodes);
		set<int,set_comparator> branchSet;
		for(int i=0;i<branchNodes.size();i++)
		{
			if(GCompare.find(branchNodes[i])==GCompare.end())
			{
				continue;
			}
			else{
				branchSet.insert(branchNodes[i]);
			}
		}
#ifdef DEBUG
		count++;
		// cerr<<"# Line: "<<count<<endl;
#endif
		formTree(ARoot,branchSet,tempLinkStore,ATableTups);
	}
#ifdef DEBUG
	cerr<<"Made tree successfully\n";
#endif
	return ;
}
void print_conditional_tree(const FPTree& root,int index)
{
	const Table & nTable = root.headTable;
	const vector<Tuple>& toot= nTable.mTable;
	FPTreeNode * temp = toot[index].linkptr;
	FPTreeNode * temp2;
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
	Printing the FP Tree
*/
void print_FPtree(const FPTree& root,vector<int>& list,int start)
{
	for(int i=start;i>=0;i--){
		cerr<<"Printing Conditional tree of ("<<list[i]<<")"<<endl;
		print_conditional_tree(root,i);
	}
}
/*
	form a conditional tree from given tree
*/
void makeFPTree(const FPTree& A,
				vector<int>& AList,
				int label,FPTree &newTree)
{
	FPTree& B = newTree;
	FPTreeNode* BRootNode = new FPTreeNode();
	B.root = BRootNode;
	Table& BTable = B.headTable;
	const vector<Tuple>& ATableTups = A.headTable.mTable;
	vector<Tuple>& BTableTups = BTable.mTable;
	int AListSize = AList.size();
	// int keyIndex = BTable.keyMap[label];
	int keyIndex = label;
	// made the new list required for the new list
	for(int i=0;i<=keyIndex;i++)
	{
		BTableTups.push_back(Tuple(ATableTups[i].key,0)); // initialize the table of the New Tree (B)
	}

	
	// links table
	FPTreeNode* ATempAcrossLink = ATableTups[keyIndex].linkptr;
	FPTreeNode* ATempVertLink; // temp to move up the links
	int NodeFreq = 0;
	vector<FPTreeNode*> BTempLinks(keyIndex+1,nullptr); // connect across links
	while(ATempAcrossLink!=nullptr)
	{
		vector<int> branchNodes;
		ATempVertLink = ATempAcrossLink;
		NodeFreq = ATempVertLink->freq;
		while(ATempVertLink->parentptr!=nullptr)
		{
			branchNodes.push_back(ATempVertLink->label); // making the set of nodes of branch
			ATempVertLink = ATempVertLink->parentptr; // moving up the links
		}
		formTree(BRootNode,branchNodes,BTempLinks,BTableTups,NodeFreq);
		ATempAcrossLink = ATempAcrossLink->linkPtr; // moving across the links
	}
#ifdef DEBUG
	cerr<<"Created tree \n";
#endif
	return ;
}


void suffixTree(vector<int>& list,vector<int> temp,int count,const FPTree& root)
{
#ifdef DEBUG
	// cerr<<"Conditional tree with nodes ";
	// for(int i=0;i<temp.size();i++)
	// 	cerr<<temp[i]<<" ";
	// cerr<<" is\n";
	// print_FPtree(root,list,count);
#endif
	for(int i = count-1;i>=0;i--)
	{
		if(root.headTable.mTable[i].freq<supportCount)
		{
			// cerr<<"For "<<root.headTable.mTable[i].key<<" ";
			// cerr<<root.headTable.mTable[i].freq<<endl;
			continue;
		}
		vector<int> temp2 = temp;
		temp2.push_back(list[i]);
		freq_sets.push_back(make_pair(temp2,root.headTable.mTable[i].freq));
		FPTree nTree;
		makeFPTree(root,list,i,nTree);
		suffixTree(list,temp2,i,nTree);
		delete_FPTree(nTree);
		// cout<<"Size "<<freq_sets.size()<<endl;
	}
	
}
void print_Frequent_sets()
{
	for(int i=0;i<freq_sets.size();i++)
	{
		for(int j=freq_sets[i].first.size()-1;j>=0;j--)
		{
			cout<<freq_sets[i].first[j]<<" ";
		}
		cout<<" ==> "<<freq_sets[i].second<<endl;
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
	supportCount = (totalTransactions*mSupportPercent+99)/100;
#ifdef DEBUG
	// supportCount = 50;
	cerr<<"# Support Count "<<supportCount<<endl;
	// for(auto a:mMap)
	// {
	// 	cerr<<a.first<<"\t"<<a.second<<endl;
	// }
#endif
	FPTree tree; // total tree //
	Table &tTable = tree.headTable; // temporary table
	unordered_map<int,int>& mKeyMap = GCompare; // contains KEY || Index
	vector<pair<int,int>> headfreqCount;
	for(auto itr = mMap.begin();itr!=mMap.end();itr++){
		if(itr->second<supportCount){
			// mMap.erase(itr);
			continue;
		}
		mKeyMap[itr->first] = 0;
		headfreqCount.push_back(make_pair(itr->first,itr->second));
	}
	sort(headfreqCount.begin(),headfreqCount.end(),mCustomSort);

#ifdef DEBUG
	cerr<<"# Printing the database after pruning the low support count"<<endl;
	for(int i=0;i<headfreqCount.size();i++)
	{
		cerr<<headfreqCount[i].first<<" "<<headfreqCount[i].second<<endl;
	}
#endif
	vector<int> sorted_list;
	int tempSize = headfreqCount.size();
	for(int i=0;i<tempSize;i++){
		mKeyMap[headfreqCount[i].first] = i;
		sorted_list.push_back(headfreqCount[i].first);
	}
#ifdef DEBUG
	cerr<<"Size of sorted array "<<sorted_list.size()<<endl;
#endif
	makeFPTreeFromDB (tree,sorted_list);
	// print_FPtree(tree,sorted_list,sorted_list.size()-1);
	vector<int> empty_list;
	cerr<<"ending"<<endl;

	suffixTree(sorted_list,empty_list,sorted_list.size(),tree);
	cerr<<"ended"<<endl;
	print_Frequent_sets();
	return 0;
}
