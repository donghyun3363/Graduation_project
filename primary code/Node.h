#pragma once
#include<iostream>
#include<fstream>
#include <cstring>
#include <ctime>
#include "linkedqueue.h"
using namespace std;

class linkedqueue;
class my_queue;
class Node
{ 
private:
	//char* time;
Node* edge[50];
	Node* Prior[50];
	int e_count[50];
	int e_index, p_index;
    
	int count;
		
public:

	char* url;
	int v_count;
	linkedqueue* queue;

	
	~Node(void);
	Node(void);
	Node(char* i_url,Node* pre);//,char*title);
	
	void insert_edge(Node* New);
	void delete_edge(Node* New);
	void set_Prior(Node* pre);
	Node* get_Prior(int);
	int get_p_index();
Node** get_edge();
int get_e_index();

	//char* title;
	//void renew_time(char* time);
	//char* get_time(void);
};

