#include <deque>
#include "ProgressCounter.h"
#include "Node.h"

using namespace std;

class Edge
{
public:

	Node* parent, * child;
	static Edge* edges;
	static int index;
	static int count;

	Edge();
	Edge(Node* a, Node* b);
	static Edge* Add(Node* a, Node* b);
	static void erase(deque<Edge*>& edges, Node* n);
	static void erase(deque<Edge*>& edges, Edge* e);
	static int swapParent(deque<Edge*>& edges, Edge* e, Node* new_parent);
	static int swapParent(deque<Edge*>& edges, Node* parent, Node* child, Node* new_parent);
	static int swapChild(deque<Edge*>& edges, Edge* e, Node* new_parent);
	static int swapChild(deque<Edge*>& edges, Node* parent, Node* child, Node* new_child);
	bool pendant();
	~Edge();
};
