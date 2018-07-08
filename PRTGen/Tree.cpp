#include <deque>
#include <random>
#include "Tree.h"
#include "ProgressCounter.h"

using namespace std;

Node::Node() : index(0) { count++; }

Node::Node(int a) : index(a) { count++; }

void Node::join(Node *a, Node *b)
{
	a->edges.push_back(b);
	b->edges.push_back(a);
}

void Node::separate(Node *a, Node *b)
{
	Node::erase(a->edges, b);
	Node::erase(b->edges, a);
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

Edge::Edge(Node *a, Node *b) : left(a), right(b) { count++; }

void Edge::erase(deque<Edge*>& edges, Node *n)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i]->left == n || edges[i]->right == n)
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
	return (left->index > 0) || (right->index > 0);
}

Edge::~Edge() { count--; }

Tree::Tree() {
	count++;
}

Tree* Tree::Equal(int N, bool rooted, bool binary, float P, ProgressCounter* pc)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine e1(rd());
	std::uniform_int_distribution<int> distribution(0, INT_MAX);
	Tree *tree = new Tree();

	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	int n = 0;
	// create root and two initial vertices
	tree->root = new Node();
	tree->nodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode;
	for (int i = 0; i < 2; i++) {
		node = new Node(label[n++]);
		tree->nodes.push_back(node);
		Node::join(tree->root, node);
		tree->edges.push_back(new Edge(tree->root, node));
	}

	while (true)
	{
		if (n == N)
			break;

		// take random edge
		Edge *edge = tree->edges[distribution(e1) % tree->edges.size()];
		// always take last edge (only for debuging)
		//edge = tree->edges[tree->edges.size() - 1];
		// always take first edge (only for debuging)
		//edge = tree->edges[0];

		// add bifurcation with 1-P probability
		if (P < static_cast<float>(distribution(e1)) / static_cast<float>(INT_MAX)) {

			// remove taken edge
			Edge::erase(tree->edges, edge);
			Node::erase(edge->left->edges, edge->right);
			Node::erase(edge->right->edges, edge->left);

			// add new internal vertex
			inode = new Node();
			tree->nodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(inode, edge->left);
			tree->edges.push_back(new Edge(inode, edge->left));
			Node::join(inode, edge->right);
			tree->edges.push_back(new Edge(inode, edge->right));

			delete edge;
		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->left;
		}

		// create new vertex
		node = new Node(label[n++]);
		Node::join(inode, node);
		tree->edges.push_back(new Edge(inode, node));
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
			tree->edges.push_back(new Edge(node, inode));
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
	Tree *tree = new Tree();

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
	tree->root = new Node();
	tree->nodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode;
	for (int i = 0; i < 2; i++) {
		node = new Node(label[n++]);
		tree->nodes.push_back(node);
		Node::join(tree->root, node);
		tree->edges.push_back(new Edge(tree->root, node));
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
			Node::erase(edge->left->edges, edge->right);
			Node::erase(edge->right->edges, edge->left);

			// add new internal vertex
			inode = new Node();
			tree->nodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(inode, edge->left);
			tree->edges.push_back(new Edge(inode, edge->left));
			Node::join(inode, edge->right);
			tree->edges.push_back(new Edge(inode, edge->right));

			delete edge;
		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->left;
		}

		// create new vertex
		node = new Node(label[n++]);
		Node::join(inode, node);
		tree->edges.push_back(new Edge(inode, node));
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
			tree->edges.push_back(new Edge(node, inode));
			// pointer to root must be set to internal vertex
			tree->root = inode;
		}
	}

	delete[] label;
	return tree;
}

void Tree::All(int N, bool rooted, bool binary, ostream& file, ProgressCounter* pc)
{
	Tree *tree = new Tree();
	/*
	int count = 1;
	for(int i = 3; i <= 2 * N - 5; i += 2)
	count *= i;
	if(rooted)
	count *= 2 * N - 3;
	file << count << endl;
	*/
	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	int n = 0;
	// create two initial vertex and join them
	tree->root = new Node(label[n++]);
	tree->nodes.push_back(tree->root);
	Node *node = new Node(label[n++]);
	Node::join(tree->root, node);
	tree->edges.push_back(new Edge(tree->root, node));
	tree->nodes.push_back(node);

	tree->Explode(n, N, rooted, binary, file, label, pc);
	if (pc) pc->updateProgress();

	delete[] label;
	tree->Delete();
}

void Tree::Explode(int n, int N, bool rooted, bool binary, ostream& file, int *label, ProgressCounter* pc)
{
	if (n == N && (!rooted || !binary))
	{
		if (pc)
		{
			pc->nextTreeCounted();
			pc->updateProgress();
		}
		Print(root, NULL, file);
		file << ";" << endl;
		if (!rooted) return;
	}

	if (n < N && !binary) {
		for (int i = 0; i < nodes.size(); i++) {
			// for all internal nodes
			if (nodes[i]->index == 0)
			{
				// create new leaf
				Node *leaf = new Node(label[n]);
				// and join it with internal node
				Node::join(nodes[i], leaf);
				Edge *second = new Edge(nodes[i], leaf);
				edges.push_back(second);
				nodes.push_back(leaf);
				Explode(n + 1, N, rooted, binary, file, label, pc);
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
		Node::erase(edge->left->edges, edge->right);
		Node::erase(edge->right->edges, edge->left);

		// create new internal verticle
		Node *node = root;
		root = new Node();

		// add join it with two removed edge neighbours
		Node::join(root, edge->left);
		Node::join(root, edge->right);

		Edge *left = new Edge(root, edge->left);
		Edge *right = new Edge(root, edge->right);
		edges.push_back(left);
		edges.push_back(right);
		nodes.push_back(root);

		if (n == N && rooted)
		{
			if (pc)
			{
				pc->nextTreeCounted();
				pc->updateProgress();
			}
			Print(root, NULL, file);
			file << ";" << endl;
		}
		else
		{
			// create new leaf and join it with new internal node
			Node *leaf = new Node(label[n]);
			Node::join(root, leaf);
			Edge *third = new Edge(root, leaf);
			edges.push_back(third);
			nodes.push_back(leaf);

			Explode(n + 1, N, rooted, binary, file, label, pc);

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

		Node::erase(edge->left->edges, root);
		Node::erase(edge->right->edges, root);

		delete root;
		root = node;

		Node::join(edge->left, edge->right);
		edges.insert(edges.begin() + i, edge);
	}
}

void Tree::Print(Node* node, Node *parent, ostream& file)
{
	deque<Node*> edges(node->edges);
	Node::erase(edges, parent);

	if (!edges.empty())
	{
		file << "(";
		for (int i = 0; i < edges.size(); i++)
		{
			Print(edges[i], node, file);
			if (i + 1 == edges.size())
				break;
			file << ",";
		}
		file << ")";
	}
	if (node->index > 0)
		file << node->index;
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