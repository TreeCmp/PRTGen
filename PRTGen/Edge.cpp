#include <deque>
#include "Tree.h"
#include "ProgressCounter.h"


Edge::Edge() : parent(), child() {}

Edge::Edge(Node* parent, Node* child) : parent(parent), child(child) { count++; }

Edge* Edge::Add(Node* parent, Node* child)
{
	edges[Edge::index].parent = parent;
	edges[Edge::index].child = child;
	return &edges[Edge::index++];
}

void Edge::erase(deque<Edge*>& edges, Node* n)
{
	for (int i = 0; i < edges.size();)
		if (edges[i]->parent == n || edges[i]->child == n)
		{
			edges.erase(edges.begin() + i);
		}
		else i++;
	return;
}

void Edge::erase(deque<Edge*>& edges, Edge* e)
{
	for (int i = 0; i < edges.size();)
		if (edges[i] == e)
		{
			Edge* toDel = edges[i];
			edges.erase(edges.begin() + i);
		}
		else i++;
	return;
}

int Edge::swapParent(deque<Edge*>& edges, Edge* e, Node* newParent)
{
	int count = 0;
	for (deque<Edge*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it == e) {
			(*it)->parent = newParent;
			count++;
		}
	}
	return count;
}

int Edge::swapParent(deque<Edge*>& edges, Node* parent, Node* child, Node* newParent)
{
	int count = 0;
	for (deque<Edge*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if ((*it)->parent == parent && (*it)->child == child) {
			(*it)->parent = newParent;
			count++;
		}
	}
	return count;
}

int Edge::swapChild(deque<Edge*>& edges, Edge* e, Node* newChild)
{
	int count = 0;
	for (deque<Edge*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if (*it == e) {
			(*it)->child = newChild;
			count++;
		}
	}
	return count;
}

int Edge::swapChild(deque<Edge*>& edges, Node* parent, Node* child, Node* newChild)
{
	int count = 0;
	for (deque<Edge*>::iterator it = edges.begin(); it != edges.end(); it++) {
		if ((*it)->parent == parent && (*it)->child == child) {
			(*it)->child = newChild;
			count++;
		}
	}
	return count;
}



bool Edge::pendant()
{
	return (parent->label > 0) || (child->label > 0);
}

Edge::~Edge() { count--; }