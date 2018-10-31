#pragma once
#include <fstream>
#include <deque>
#include "ProgressCounter.h"

using namespace std;

class Node
{
public:

	// labels from 1 to N, internal nodes label equals 0
	int label;
	deque<Node*> edges;
	Node* parent;
	int siblingNumber;
	static Node* nodes;
	static int index;
	static int count;

	Node(Node* = NULL);
	Node(int a, Node* = NULL);
	static Node* Add(Node* = NULL);
	static Node* Add(int a, Node* = NULL);
	static void join(Node *a, Node *b);
	static void separate(Node *a, Node *b);
	static void erase(deque<Node*>& edges, Node *a);
	~Node();
};

class Edge
{
public:

	Node *parent, *child;
	static Edge* edges;
	static int index;
	static int count;

	Edge();
	Edge(Node *a, Node *b);
	static Edge* Add(Node *a, Node *b);
	static void erase(deque<Edge*>& edges, Node *n);
	static void erase(deque<Edge*>& edges, Edge *e);
	bool pendant();
	~Edge();
};

class Tree
{

public:

	static int N;
	Node *root;
	deque<Edge*> edges;
	deque<Node*> nodes;
	float P;
	bool generateAllTrees;
	int internalNodesNumberExpected;
	int internalNodesNumber;
	int sackinIndexValue;
	int remainingCaterpillarCaseValue;
	int remainingBalancedTreeCaseValue;
	static bool rooted;
	static bool binary;
	static int count;
	static bool printIndexes;
	static double minSackinsIndex, maxSackinsIndex;
	static char sackin_norm_model;
	static double sum;

	Tree(int N, bool rooted, bool binary, float P);
	void CountExtremeSackinIndexValues(int n);
	static Tree* Equal(int N, bool rooted, bool binary, float P, ProgressCounter* pc);
	static Tree* Yule(int N, bool rooted, bool binary, float P, ProgressCounter* pc);
	static void All(int N, bool rooted, bool binary, ostream& file, int P, ProgressCounter* pc);
	void Explode(int n, int N, Tree* tree, ostream& file, int *label, ProgressCounter* pc);
	static bool Print(Node* node, Node *parent, ostream& file, ProgressCounter* pc);
	static void CountSackinIndex(Node* node, Node *parent, int& sackinInd, int depth = 0);
	static double NormalizeSackinIndex(int sackinInd);
	static void PrintRec(Node* node, Node *parent, ostream& file);
	static void CountSum();
	void ClearAndDelete();
	void Delete();
	~Tree();
};