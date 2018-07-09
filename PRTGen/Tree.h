#pragma once
#include <fstream>
#include <deque>
#include "ProgressCounter.h"

using namespace std;

class Node
{
public:

	int index;
	deque<Node*> edges;
	static int count;

	Node();
	Node(int a);
	static void join(Node *a, Node *b);
	static void separate(Node *a, Node *b);
	static void erase(deque<Node*>& edges, Node *a);
	~Node();
};

class Edge
{
public:

	Node *left, *right;
	static int count;

	Edge(Node *a, Node *b);
	static void erase(deque<Edge*>& edges, Node *n);
	static void erase(deque<Edge*>& edges, Edge *e);
	bool pendant();
	~Edge();
};

class Tree
{
public:

	Node *root;
	deque<Edge*> edges;
	deque<Node*> nodes;
	bool rooted;
	bool binary;
	float P;
	bool generateAllTrees;
	int internalNodesNumberExpected;
	int internalNodesNumber;
	static int count;

	Tree(bool rooted, bool binary, float P);
	static Tree* Equal(int N, bool rooted, bool binary, float P, ProgressCounter* pc);
	static Tree* Yule(int N, bool rooted, bool binary, float P, ProgressCounter* pc);
	static void All(int N, bool rooted, bool binary, ostream& file, int P, ProgressCounter* pc);
	void Explode(int n, int N, Tree* tree, ostream& file, int *label, ProgressCounter* pc);
	static void Print(Node* node, Node *parent, ostream& file);
	void Delete();
	~Tree();
};