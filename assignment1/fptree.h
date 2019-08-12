#ifndef FPTREE_H
#define FPTREE_H
#include <unordered_map>
#include <vector>
using namespace std;
class FPTreeNode {
	public:
        int label;
		int freq;
		unordered_map<int,FPTreeNode*> children;
		FPTreeNode *parentptr;
		FPTreeNode *linkPtr;
	public:
		FPTreeNode():parentptr(nullptr)
			     ,linkPtr(nullptr)
			     ,freq(-1)
		{}
        FPTreeNode(int lab,int fr,FPTreeNode* par):label(lab),freq(fr),parentptr(par),linkPtr(nullptr)
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

class Table{
    public:
        unordered_map<int,int> keyMap;
        vector<Tuple> mTable;

};

class FPTree{
	public:
		FPTreeNode* root;
		Table headTable;
	public:
		FPTree()
		{}
		void makeFPTree()
		{
			
		}
};


#endif
