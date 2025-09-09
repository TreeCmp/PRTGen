#include <deque>
#include <random>
#include "Tree.h"
#include "ProgressCounter.h"

using namespace std;

Node::Node(Node* parent) : label(0), weight(NULL), siblingNumber(0)
{
	this->parent = parent;
	count++;
}

Node::Node(int a, char* weight, Node* parent) : label(a), weight(weight), siblingNumber(0)
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

void Node::join(Node* parent, Node* child)
{
	parent->edges.push_back(child);
	child->edges.push_back(parent);
	child->parent = parent;
	if (parent->degree() > 3 || child->degree() > 3) {
		int debug = 1;
	}
}

void Node::joinIfNotContains(Node* parent, Node* child)
{
	if (!parent->ContainsNeigbour(child)) {
		parent->edges.push_back(child);
	}
	if (!child->ContainsNeigbour(parent)) {
		child->edges.push_back(parent);
	}
	child->parent = parent;
}

void Node::separate(Node* parent, Node* child)
{
	Node::erase(parent->edges, child);
	Node::erase(child->edges, parent);
	child->parent = NULL;
}

void Node::erase(deque<Node*>& edges, Node* a)
{
	for (int i = 0; i < edges.size(); i++)
		if (edges[i] == a)
		{
			edges.erase(edges.begin() + i);
			return;
		}
}

int Node::degree()
{
	if (Tree::rooted) {
		deque<Node*>::iterator it = find(this->edges.begin(), this->edges.end(), this->parent);
		return this->edges.size() + (it == this->edges.end() ? 1 : 0);
	}
	else {
		return this->edges.size();
	}
}

Node* Node::takeFirstOtherChild(Node* forbiddenChild) {
	for (deque<Node*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it != this->parent && *it != forbiddenChild) {
			return *it;
		}
	}
}

Node* Node::takeSecondOtherChild(Node* forbiddenChild) {
	bool second = false;
	for (deque<Node*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it != this->parent && *it != forbiddenChild) {
			if (second) {
				return *it;
			}
			else {
				second = true;
			}
		}
	}
}

Node* Node::takeRandomOtherChild(Node* forbiddenChild) {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine eng1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	Node* otherRandomChild = NULL;
	do
	{
		otherRandomChild = edges[range_0_INTMAX_unif_int_distr(eng1) % edges.size()];
	} while (otherRandomChild == forbiddenChild);
	return otherRandomChild;
}

Node* Node::takeFirstOtherNeigbour(Node* forbiddenNeigbour) {
	for (deque<Node*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it != forbiddenNeigbour) {
			return *it;
		}
	}
}

Node* Node::takeSecondOtherNeigbour(Node* forbiddenNeigbour) {
	bool second = false;
	for (deque<Node*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it != forbiddenNeigbour) {
			if (second) {
				return *it;
			}
			else {
				second = true;
			}
		}
	}
}

bool Node::ContainsNeigbour(Node* wanted) {
	if (!this->edges.empty())
	{
		for (int i = 0; i < this->edges.size(); i++)
		{
			if (this->edges[i] == wanted) {
				return true;
			}
			if (i + 1 == this->edges.size())
				break;
		}
	}
	return false;
}

Node::~Node() {
	delete weight;
	count--;
}