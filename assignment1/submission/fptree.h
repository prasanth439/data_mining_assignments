#ifndef FPTREE_H
#define FPTREE_H
#include <unordered_map>
#include <vector>
using namespace std;


template <typename T>
class ListNode{
	public:
		T data;
		ListNode<T>*  next;
		ListNode(T d):next(nullptr){
			data = d;
		}
		ListNode(T d,ListNode<T>* n):data(d),next(n)
		{}
		~ListNode()
		{
			if(next!=nullptr)
			{
				delete next;
			}
		}
};
template <typename T>
class LinkedList{
	public:
		ListNode<T>* head;
		ListNode<T>* tail;
		int size;
	public:
		LinkedList():head(nullptr),tail(nullptr)
		{
			size = 0;
		}
		void push_back(T data)
		{
			if(size==0)
			{
				head = tail = new ListNode<T>(data);
				size++;
				return ;
			}
			ListNode<T>* temp = new ListNode<T>(data);
			tail->next = temp;
			tail = temp;
			size++;
			return;
		}
		void push_front(T data)
		{
			if(size==0)
			{
				head = tail = new ListNode<T>(data);
				size++;
				return ;
			}
			ListNode<T>* temp = new ListNode<T>(data,head);
			head = temp;
			size++;
		}
		~LinkedList()
		{
			delete head;
		}
};



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
        // unordered_map<int,int> keyMap;
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

using FileName = string;
using Items = vector<int>;
using Frequency = int;
template<typename T>
using FrequentItemList = LinkedList<T>;
template <typename T,typename U>
using HashTable = unordered_map<T,U>;
#endif
