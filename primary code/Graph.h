#pragma once
#include "Node.h"
	   
class Graph
{
private:
	Node* head;
	Node* temp_node;
public:
	
	Node* arr_url[5000];
    	linkedqueue* G_queue;
	
	int a_index;
	Graph(void);
	~Graph(void);

	//Node* search(char* i_url);
	Node* count(Node* i_prior,char* i_url);
	void discount(char* i_url);
 
	void QuickSort(int Left,int Right);
	int Partition(int Left,int Right);
	void Swap(int l, int r);
	int BinarySearch(int Size,char* i_url);
	Node* gethead();

        void set_temp_node(Node* temp_n);
	Node* get_temp_node();


};
