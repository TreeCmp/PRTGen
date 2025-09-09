#pragma once

#include <deque>
#include "ProgressCounter.h"

using namespace std;

class Node
{
public:

	// labels from 1 to N, internal nodes label equals 0
	int label;
	char* weight;
	deque<Node*> edges;
	Node* parent;
	int siblingNumber;
	static Node* nodes;
	static int index;
	static int count;

	Node(Node* = NULL);
	Node(int a, char* weight = NULL, Node* = NULL);
	static Node* Add(Node* = NULL);
	static Node* Add(int a, Node* = NULL);
	static void join(Node* a, Node* b);
	static void joinIfNotContains(Node* a, Node* b);
	static void separate(Node* a, Node* b);
	static void erase(deque<Node*>& edges, Node* a);
	int Node::degree();
	Node* takeFirstOtherChild(Node* n);
	Node* takeSecondOtherChild(Node* n);
	Node* takeRandomOtherChild(Node* n);
	Node* takeFirstOtherNeigbour(Node* n);
	Node* takeSecondOtherNeigbour(Node* n);
	bool ContainsNeigbour(Node* wanted);
	~Node();
};