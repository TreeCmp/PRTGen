#include <deque>
#include <random>
#include <math.h>
#include "Tree.h"
#include "ProgressCounter.h"

using namespace std;

Node::Node(Node* parent) : label(0), siblingNumber(0)
{
	this->parent = parent;
	count++;
}

Node::Node(int a, Node* parent) : label(a), siblingNumber(0)
{
	this->parent = parent;
	count++;
}

Node* Node::Add(Node* parent)
{
	nodes[Node::index].label = 0;
	nodes[Node::index].siblingNumber = 0;
	nodes[Node::index].parent = parent;
	return &nodes[Node::index++];
}

Node* Node::Add(int a, Node* parent)
{
	nodes[Node::index].label = a;
	nodes[Node::index].siblingNumber = 0;
	nodes[Node::index].parent = parent;
	return &nodes[Node::index++];
}

void Node::join(Node *parent, Node *child)
{
	parent->edges.push_back(child);
	child->edges.push_back(parent);
	child->parent = parent;
}

void Node::separate(Node *parent, Node *child)
{
	Node::erase(parent->edges, child);
	Node::erase(child->edges, parent);
	child->parent = NULL;
}

void Node::erase(deque<Node*>& edges, Node *a)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i] == a)
		{
			edges.erase(edges.begin() + i);
			return;
		}
}

Node::~Node() { count--; }

Edge::Edge() : parent(), child() {}

Edge::Edge(Node *a, Node *b) : parent(a), child(b) { count++; }

Edge* Edge::Add(Node *a, Node *b)
{
	edges[Edge::index].parent = a;
	edges[Edge::index].child = b;
	return &edges[Edge::index++];
}

void Edge::erase(deque<Edge*>& edges, Node *n)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i]->parent == n || edges[i]->child == n)
		{
			edges.erase(edges.begin() + i);
			return;
		}
}

void Edge::erase(deque<Edge*>& edges, Edge *e)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i] == e)
		{
			edges.erase(edges.begin() + i);
			return;
		}
}

bool Edge::pendant()
{
	return (parent->label > 0) || (child->label > 0);
}

Edge::~Edge() { count--; }

int Tree::N;
double Tree::sum;

Tree::Tree(int N, bool rooted, bool binary, float P) {
	this->N = N;
	this->rooted = rooted;
	this->binary = binary;
	this->P = P;
	this->internalNodesNumberExpected = P;
	this->generateAllTrees = !(bool)P;
	this->internalNodesNumber = 0;
	this->sackinIndexValue = 0;
	//CountExtremeSackinIndexValues(0);
	count++;
}

void Tree::CountSum() {
	Tree::sum = 0.0;
	for (int j = 2; j <= Tree::N; j++) Tree::sum += 1.0 / j;
	return;
}

void Tree::CountExtremeSackinIndexValues(int n)
{
	// sackin index value growth after attaching current tree and balanced tree build of remaining nodes to root
	// equals current tree nodes number + remaining nodes number * balanced tree height + numbre of leaves at a height+1 * 2
	this->remainingBalancedTreeCaseValue = n + (N - n) * floor(log2(N - n)) + ((N - n) - pow(2, floor(log2(N - n))) * 2);
	// sackin index value growth after attaching current tree to caterpillar at N - n vertices
	// equals sackin index for caterpillar without one leg at (N - n) vertices + caterpillar length * n 
	this->remainingCaterpillarCaseValue = (((N - n) * (N - n) + (N - n)) / 2) + (N - n) * n;
}

Tree* Tree::Equal(int N, bool rooted, bool binary, float P, ProgressCounter* pc)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine e1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	Tree *tree = new Tree(N, rooted, binary, P);

	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	int n = 0;
	// create root and two initial vertices
	tree->root = Node::Add();

	tree->nodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode, *tmpnode;
	for (int i = 0; i < 2; i++) {
		node = Node::Add(label[n++], tree->root);

		tree->nodes.push_back(node);
		Node::join(tree->root, node);
		tree->edges.push_back(Edge::Add(tree->root, node));

	}
	// Sackin index is defined only for rooted trees
	if (tree->rooted)
	{
		tree->root->siblingNumber = 2;
		tree->sackinIndexValue = 2;
	}

	while (true)
	{
		if (n == N)
			break;
		Edge *edge;
		// take random edge
		edge = tree->edges[range_0_INTMAX_unif_int_distr(e1) % tree->edges.size()];
		// always take last edge (only for debuging)
		//edge = tree->edges[tree->edges.size() - 1];
		// always take first edge (only for debuging)
		//edge = tree->edges[0];

		// add bifurcation with 1-P probability
		if (P < static_cast<float>(range_0_INTMAX_unif_int_distr(e1)) / static_cast<float>(INT_MAX)) {

			// remove taken edge
			Edge::erase(tree->edges, edge);
			Node::erase(edge->parent->edges, edge->child);
			Node::erase(edge->child->edges, edge->parent);

			// add new internal vertex
			inode = Node::Add();

			tree->nodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(edge->parent, inode);
			tree->edges.push_back(Edge::Add(edge->parent, inode));

			Node::join(inode, edge->child);
			tree->edges.push_back(Edge::Add(inode, edge->child));

		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->parent;
		}

		if (rooted)
		{
			// compute new internal vertex siblingsNumber value
			inode->siblingNumber = edge->child->siblingNumber + 1;
			// Sackin index value update
			tree->sackinIndexValue += inode->siblingNumber;

			// increment all ancestors siblingsNumber and sackinIndexValue
			tmpnode = edge->parent;
			while (tmpnode != tree->root)
			{
				tmpnode->siblingNumber++;
				tmpnode = tmpnode->parent;
				tree->sackinIndexValue++;
			}
			tree->root->siblingNumber++;
			tree->sackinIndexValue+=2;

			// Update extreme Sackin index values
			// check that value of the sarcine index is within proper range
			tree->CountExtremeSackinIndexValues(n+1);
			if (Tree::NormalizeSackinIndex(tree->sackinIndexValue + tree->remainingBalancedTreeCaseValue) > Tree::maxSackinsIndex)
			{
				//pc->resetElapsed_secs();
				//cout << "Rejected due to sackin index value = " << Tree::NormalizeSackinIndex(tree->sackinIndexValue + tree->remainingBalancedTreeCaseValue) << " > " << Tree::maxSackinsIndex << endl;
				tree->ClearAndDelete();
				delete[] label;
				if (pc) pc->updateProgress(n);
				return NULL;
			}
			if (Tree::NormalizeSackinIndex(tree->sackinIndexValue + tree->remainingCaterpillarCaseValue) < Tree::minSackinsIndex)
			{
				//pc->resetElapsed_secs();
				//cout << "Rejected due to sackin index value = " << Tree::NormalizeSackinIndex(tree->sackinIndexValue + tree->remainingCaterpillarCaseValue) << " < " << Tree::minSackinsIndex << endl;
				tree->ClearAndDelete();
				delete[] label;
				if (pc) pc->updateProgress(n);
				return NULL;
			}
		}

		// create new vertex
		node = Node::Add(label[n++]);

		Node::join(inode, node);
		tree->edges.push_back(Edge::Add(inode, node));

		tree->nodes.push_back(node);

		if (pc) pc->updateProgress(n);
	}

	// shrink root verticle
	if (!rooted) {
		if (tree->root->edges.size() == 2) {
			// take two root neighbours
			node = tree->root->edges[0];
			inode = tree->root->edges[1];
			// remove root with two incident edges
			Edge::erase(tree->edges, tree->root);
			Node::separate(node, tree->root);
			Node::separate(inode, tree->root);
			// add new edge between root neighbours
			Node::join(node, inode);
			tree->edges.push_back(Edge::Add(node, inode));

			// pointer to root must be set to internal vertex
			tree->root = inode;
		}
	}

	delete[] label;
	return tree;
}

Tree*Tree::Yule(int N, bool rooted, bool binary, float P, ProgressCounter* pc)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine e1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	std::uniform_real_distribution<float> range_0_1_unif_float_distr(0, 1);
	Tree *tree = new Tree(N, rooted, binary, P);

	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	// permutate labels
	for (int i = N - 1; i >= 1; i--)
	{
		int index = range_0_INTMAX_unif_int_distr(e1) % (i + 1);
		swap(label[index], label[i]);
	}

	int n = 0;
	// create root and two initial vertices
	tree->root = Node::Add();
	tree->nodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode;
	for (int i = 0; i < 2; i++) {
		node = Node::Add(label[n++]);
		tree->nodes.push_back(node);
		Node::join(tree->root, node);
		tree->edges.push_back(Edge::Add(tree->root, node));
	}

	while (true)
	{
		if (n == N)
			break;

		// take random pending edge (at least one vertex with non-zero index)
		Edge *edge;
		do
		{
			edge = tree->edges[range_0_INTMAX_unif_int_distr(e1) % tree->edges.size()];
			// always take last edge (only for debuging)
			//edge = tree->edges[tree->edges.size() - 1];
			// always take first edge (only for debuging)
			//edge = tree->edges[0];
		} while (edge->pendant() == false && !(n == N && rooted));

		// add bifurcation with 1-P probability
		if (P < static_cast<float>(range_0_1_unif_float_distr(e1)) / static_cast<float>(INT_MAX)) {

			// remove taken edge
			Edge::erase(tree->edges, edge);
			Node::erase(edge->parent->edges, edge->child);
			Node::erase(edge->child->edges, edge->parent);

			// add new internal vertex
			inode = Node::Add();
			tree->nodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(inode, edge->parent);
			tree->edges.push_back(Edge::Add(inode, edge->parent));
			Node::join(inode, edge->child);
			tree->edges.push_back(Edge::Add(inode, edge->child));

		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->parent;
		}

		// create new vertex
		node = Node::Add(label[n++]);
		Node::join(inode, node);
		tree->edges.push_back(Edge::Add(inode, node));
		tree->nodes.push_back(node);

		if (pc) pc->updateProgress(n);
	}

	// shrink root verticle
	if (!rooted) {
		if (tree->root->edges.size() == 2) {
			// take two root neighbours
			node = tree->root->edges[0];
			inode = tree->root->edges[1];
			// remove root with two incident edges
			Edge::erase(tree->edges, tree->root);
			Node::separate(node, tree->root);
			Node::separate(inode, tree->root);
			// add new edge between root neighbours
			Node::join(node, inode);
			tree->edges.push_back(Edge::Add(node, inode));
			// pointer to root must be set to internal vertex
			tree->root = inode;
		}
	}

	delete[] label;
	return tree;
}

void Tree::All(int N, bool rooted, bool binary, ostream& file, int P, ProgressCounter* pc)
{
	Tree *tree = new Tree(N, rooted, binary, P);
	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	int n = 0;
	// create two initial vertex and join them
	tree->root = new Node(label[n++]);
	tree->nodes.push_back(tree->root);
	Node *node = new Node(label[n++]);
	tree->nodes.push_back(node);
	Node::join(tree->root, node);
	tree->edges.push_back(new Edge(tree->root, node));

	tree->Explode(n, N, tree, file, label, pc);
	if (pc) pc->updateProgress();

	delete[] label;
	tree->Delete();
}

void Tree::Explode(int n, int N, Tree* tree, ostream& file, int *label, ProgressCounter* pc)
{
	if (pc)
	{
		pc->updateProgress();
	}
	if (n == N && (!rooted || !binary))
	{
		if (rooted && root->edges.size() && (generateAllTrees || internalNodesNumber == internalNodesNumberExpected))
		{
			for (int i = 0; i < nodes.size(); i++) {
				// for all internal nodes
				if (nodes[i]->label == 0)
				{
					if (pc)
					{
						pc->nextTreeCounted();
					}
					Tree::Print(nodes[i], NULL, file, pc);
				}
			}
		}
		else if (generateAllTrees || internalNodesNumber == internalNodesNumberExpected)
		{
			if (pc)
			{
				pc->nextTreeCounted();
			}
			Tree::Print(root, NULL, file, pc);
		}
		if (!rooted) return;
	}

	if (n < N && !binary) {
		for (int i = 0; i < nodes.size(); i++) {
			// for all internal nodes
			if (nodes[i]->label == 0)
			{
				// create new leaf
				Node *leaf = new Node(label[n]);
				// and join it with internal node
				Node::join(nodes[i], leaf);
				Edge *second = new Edge(nodes[i], leaf);
				edges.push_back(second);
				nodes.push_back(leaf);
				if ((generateAllTrees || internalNodesNumber <= internalNodesNumberExpected))
				{
					Explode(n + 1, N, tree, file, label, pc);
				}
				// remove all changes
				nodes.pop_back();
				edges.pop_back();
				delete second;
				Node::erase(nodes[i]->edges, leaf);
				delete leaf;
			}

		}
	}

	for (int i = 0; i < edges.size(); i++)
	{
		Edge *edge = edges[i];

		// remove edge
		Edge::erase(edges, edge);
		Node::erase(edge->parent->edges, edge->child);
		Node::erase(edge->child->edges, edge->parent);

		// create new internal verticle
		Node *node = root;
		root = new Node();

		// add join it with two removed edge neighbours
		Node::join(root, edge->parent);
		Node::join(root, edge->child);
		internalNodesNumber++;

		Edge *left = new Edge(root, edge->parent);
		Edge *right = new Edge(root, edge->child);
		edges.push_back(left);
		edges.push_back(right);
		nodes.push_back(root);

		if (n == N && (generateAllTrees || internalNodesNumber == internalNodesNumberExpected))
		{
			if (pc)
			{
				pc->nextTreeCounted();
			}
			Tree::Print(root, NULL, file, pc);
		}
		else
		{
			// create new leaf and join it with new internal node
			Node *leaf = new Node(label[n]);
			Node::join(root, leaf);
			Edge *third = new Edge(root, leaf);
			edges.push_back(third);
			nodes.push_back(leaf);
			if ((generateAllTrees || internalNodesNumber <= internalNodesNumberExpected))
			{
				Explode(n + 1, N, tree, file, label, pc);
			}
			nodes.pop_back();
			edges.pop_back();
			delete third;
			Node::erase(leaf->edges, root);
			delete leaf;
		}

		// remove all changes
		nodes.pop_back();
		edges.pop_back();
		edges.pop_back();
		delete left;
		delete right;

		Node::erase(edge->parent->edges, root);
		Node::erase(edge->child->edges, root);

		delete root;
		root = node;

		internalNodesNumber--;

		Node::join(edge->parent, edge->child);
		edges.insert(edges.begin() + i, edge);
	}
}

void Tree::CountSackinIndex(Node* node, Node *parent, int& sackinInd, int depth) {
	Node::erase(node->edges, parent);

	if (!node->edges.empty())
	{
		for (int i = 0; i < node->edges.size(); i++)
		{
			CountSackinIndex(node->edges[i], node, sackinInd, depth + 1);
			if (i + 1 == node->edges.size())
				break;
		}
	}
	if (node->label > 0) {
		sackinInd += depth;
	}
}

double Tree::NormalizeSackinIndex(int sackinInd)
{
	switch (Tree::sackin_norm_model)
	{
	case 'n':
		return (double)2*(sackinInd - N) / ((N * N) - N - 2);
	case 'y':
		return (double)((double)sackinInd - 2.0 * N * Tree::sum) / N;
	case 'e':
		return (double)sackinInd / (double)pow(N, 1.5);
	}
}

bool Tree::Print(Node* node, Node *parent, ostream& file, ProgressCounter* pc)
{
	int sackinInd = 0;
	double normSackinInd = 0.0;
	Tree::CountSackinIndex(node, NULL, sackinInd);
	normSackinInd = Tree::NormalizeSackinIndex(sackinInd);

	if (Tree::minSackinsIndex <= normSackinInd && Tree::maxSackinsIndex >= normSackinInd) {
		Tree::PrintRec(node, NULL, file);
		file << ";";
		if (printIndexes && Tree::rooted) {
			file << " I_s = " << sackinInd << "; |I_s| = " << normSackinInd;
		}
		file << endl;
		return true;
	}
	else {
		//throw "wrongly filtered tree";
		//pc->resetElapsed_secs();
		//if (Tree::minSackinsIndex <= normSackinInd) cout << "rejected due to too high sackin index value = " << normSackinInd;
		//else cout << "rejected due to too low sackin index value = " << normSackinInd;
		//cout << endl;
		return false;
	}
}

void Tree::PrintRec(Node* node, Node *parent, ostream& file)
{
	deque<Node*> edges(node->edges);
	Node::erase(edges, parent);

	if (!edges.empty())
	{
		file << "(";
		for (int i = 0; i < edges.size(); i++)
		{
			PrintRec(edges[i], node, file);
			if (i + 1 == edges.size())
				break;
			file << ",";
		}
		file << ")";
	}
	if (node->label > 0) {
		file << node->label;
	}
}

void Tree::ClearAndDelete()
{
	while (!nodes.empty())
	{
		Node *node = nodes.back();
		nodes.pop_back();
		node->edges.clear();
	}
	edges.clear();
	Node::index = 0;
	Edge::index = 0;
	delete this;
}

void Tree::Delete()
{
	while (!edges.empty())
	{
		Edge *edge = edges.back();
		edges.pop_back();
		delete edge;
	}

	while (!nodes.empty())
	{
		Node *node = nodes.back();
		nodes.pop_back();
		delete node;
	}

	delete this;
}

Tree::~Tree() { count--; }