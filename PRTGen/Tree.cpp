#include <deque>
#include <random>
#include <math.h>
#include "Tree.h"
#include "ProgressCounter.h"

int Tree::N;
double Tree::sum;

Tree::Tree(Parameters param) {
	this->N = param.N;
	this->rooted = param.rooted;
	this->binary = param.binary;
	this->P = param.P;
	this->internalNodesNumberExpected = param.P;
	this->generateAllTrees = !(bool)param.P;
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

bool Tree::isAncestor(Node* n1, Node* n2) {
	Node* tmp_node = n2;
	while (tmp_node && tmp_node != this->root) {
		if (tmp_node == n1) {
			return true;
		}
		tmp_node = tmp_node->parent;
	}
	return false;
}

bool Tree::isValidNNI(Edge* s) {
	if (s->parent->degree() < 2) {
		return false;
	}
	if (s->child->degree() < 2) {
		return false;
	}
	return true;
}


bool Tree::isValidSPR(Edge* s, Edge* t) {
	if (s == t) {
		return false;
	}
	if (s->parent == t->parent) {
		return false;
	}
	if (s->child == t->parent || s->parent == t->child) {
		return false;
	}
	if (isAncestor(s->child, t->parent)) {
		return false;
	}
	return true;
}

bool Tree::areNeighbours(Node* s, Node* t) {
	for (deque<Node*>::iterator it = s->edges.begin(); it != s->edges.end(); it++) {
		if (*it == t) {
			//for debuging only
			bool check = false;
			for (deque<Node*>::iterator it = t->edges.begin(); it != t->edges.end(); it++) {
				if (*it == s) {
					check = true;
				}
			}
			if (!check) {
				throw new exception("missing neighbor");
			}
			return true;
		}
	}
	return false;
}

bool Tree::isCloserThan3(Edge* s, Edge* t) {
	if (areNeighbours(s->parent, t->parent) ||
		areNeighbours(s->child, t->parent) ||
		areNeighbours(s->parent, t->child) || 
		areNeighbours(s->child, t->child)) {
		return true;
	}
	return false;
}

bool Tree::isValidUSPR(Edge* s, Edge* t) {
	if (s == t) {
		return false;
	}
	if (s->parent == t->parent || s->child == t->child) {
		return false;
	}
	if (s->child == t->parent || s->parent == t->child) {
		return false;
	}
	return true;
}

bool Tree::DoNNI() {
	if (!iSTreeSizeOKForRearrangement(this))
	{
		return false;
	}
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine eng1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	Edge* e = NULL;
	// take one random internal edge
	do {
		e = this->allEdges[range_0_INTMAX_unif_int_distr(eng1) % this->allEdges.size()];
	} while (!isValidNNI(e));
	Node* u = NULL;
	do {
		u = e->parent->takeRandomOtherChild(e->child);
	} while (u == e->parent || u->parent != e->parent);
	Node* v = NULL;
	do {
		v = e->child->edges[range_0_INTMAX_unif_int_distr(eng1) % e->child->edges.size()];
	} while (v == e->parent);
	// swapping 2 edges (eParent, u) and (eChild, v), [notation: (parent, child)]
	if (e->parent != u->parent)
	{
		throw exception("e->parent isn't u parent");
	};
	Node::separate(e->parent, u);
	if (e->child != v->parent)
	{
		throw exception("e->parent isn't v parent");
	};
	Node::separate(e->child, v);
	Node::join(e->child, u);
	Node::join(e->parent, v);
	int swapedCount = 0;
	swapedCount = Edge::swapParent(this->allEdges, e->parent, u, e->child);
	if (swapedCount != 1)
	{
		throw exception(swapedCount + "is wrong swapped nodes number");
	};
	swapedCount = Edge::swapParent(this->allEdges, e->child, v, e->parent);
	if (swapedCount != 1)
	{
		throw exception(swapedCount + "is wrong swapped nodes number");
	};
	return true;
}

bool Tree::DoSPR() {
	if (!iSTreeSizeOKForRearrangement(this))
	{
		return false;
	}
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine eng1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	Edge *e1 = NULL;
	Edge *e2 = NULL;
	if (rooted) {
		// take two different random edges
		do {
			e1 = this->allEdges[range_0_INTMAX_unif_int_distr(eng1) % this->allEdges.size()];
			e2 = this->allEdges[range_0_INTMAX_unif_int_distr(eng1) % this->allEdges.size()];
			//todo: dodaæ usuwanie nadmiarowych NNI
		} while (!isValidSPR(e1, e2));
		Node* e1_child_brother = e1->parent->takeFirstOtherChild(e1->child);
		if (e1->parent == this->root) {
			Node::separate(e1->parent, e1_child_brother);
			this->root = e1_child_brother;
			Node::separate(e2->parent, e2->child);
			Node::join(e2->parent, e1->parent);
			Node::join(e1->parent, e2->child);
			Edge::swapChild(this->allEdges, e1->parent, e1_child_brother, e2->child);
			Edge::swapChild(this->allEdges, e2, e1->parent);
		}
		else {
			Node* e1_grand_parent = e1->parent->parent;
			int deg = e1->parent->degree();
			//condition for multifurcations occurrence possibility
			if (deg <= 3) {
				Node::separate(e1->parent, e1_child_brother);
				Node::separate(e1_grand_parent, e1->parent);
				Node::join(e1_grand_parent, e1_child_brother);
				Edge::swapParent(this->allEdges, e1->parent, e1_child_brother, e1_grand_parent);
			}
			Node::separate(e2->parent, e2->child);
			Node::join(e2->parent, e1->parent);
			Node::join(e1->parent, e2->child);
			Edge::swapParent(this->allEdges, e1_grand_parent, e1->parent, e2->parent);
			Edge::swapParent(this->allEdges, e2, e1->parent);
		}
	}
	else {
		// take two different random edges
		do {
			e1 = this->allEdges[range_0_INTMAX_unif_int_distr(eng1) % this->allEdges.size()];
			e2 = this->allEdges[range_0_INTMAX_unif_int_distr(eng1) % this->allEdges.size()];
			// the second condition is to fulfill the uniform distribution
		} while (!isValidUSPR(e1, e2) || isCloserThan3(e1, e2) && range_0_INTMAX_unif_int_distr(eng1) > INT_MAX / 4);
		Node* e1_parent = NULL;
		Node* e1_child = NULL;
		if (Tree::ContainsNode(e1->child, e1->parent, e2->parent)) {
			e1_parent = e1->child;
			e1_child = e1->parent;
		}
		if (Tree::ContainsNode(e1->parent, e1->child, e2->parent)) {
			e1_parent = e1->parent;
			e1_child = e1->child;
		}
		Node* e2_parent = NULL;
		Node* e2_child = NULL;
		if (Tree::ContainsNode(e2->child, e2->parent, e1->parent)) {
			e2_parent = e2->child;
			e2_child = e2->parent;
		}
		if (Tree::ContainsNode(e2->parent, e2->child, e1->parent)) {
			e2_parent = e2->parent;
			e2_child = e2->child;
		}
		Node* e1_node_first_neigbour = e1_parent->takeFirstOtherNeigbour(e1_child);
		Node* e1_node_second_neigbour = e1_parent->takeSecondOtherNeigbour(e1_child);
		int deg = e1_parent->degree();
		//condition for multifurcations occurrence possibility
		if (deg <= 3) {
			Node::separate(e1_parent, e1_node_second_neigbour);
			Node::separate(e1_parent, e1_node_first_neigbour);
			Node::join(e1_node_second_neigbour, e1_node_first_neigbour);
			if (!Edge::swapParent(this->allEdges, e1_parent, e1_node_first_neigbour, e1_node_second_neigbour)){
				Edge::swapChild(this->allEdges, e1_node_first_neigbour, e1_parent, e1_node_second_neigbour);
				}
		}
		Node::separate(e2_parent, e2_child);
		Node::join(e2_parent, e1_parent);
		Node::join(e1_parent, e2_child);
		if (!Edge::swapChild(this->allEdges, e1_parent, e1_node_second_neigbour, e2_parent)) {
			Edge::swapParent(this->allEdges, e1_node_second_neigbour, e1_parent, e2_parent);
		}
		if (!Edge::swapChild(this->allEdges, e2_child, e2_parent, e1_parent)) {
			Edge::swapParent(this->allEdges, e2_parent, e2_child, e1_parent);
		}
	}
	return true;
}

bool Tree::DoTBR() {
	if (!iSTreeSizeOKForRearrangement(this))
	{
		return false;
	}
	throw exception("TBR distance not implemented yet");
	return true;
}

bool Tree::iSTreeSizeOKForRearrangement(Tree* tree)
{
	if (tree->rooted && tree->allNodes.size() < 3 || tree->allNodes.size() < 4)
	{
		return false;
	}
	return true;
}

bool Tree::ContainsNode(Node* node, Node* excepted_neighbor, Node* wanted) {
	deque<Node*> edges(node->edges);
	Node::erase(edges, excepted_neighbor);

	if (!edges.empty())
	{
		for (int i = 0; i < edges.size(); i++)
		{
			if (edges[i] == wanted) {
				return true;
			}
			if (ContainsNode(edges[i], node, wanted)) {
				return true;
			}
			if (i + 1 == edges.size())
				break;
		}
	}
	return false;
}

void Tree::CountExtremeSackinIndexValues(int n)
{
	// sackin index value growth after attaching current tree and balanced tree build of remaining nodes to root
	// todo bad estimation
	// equals current tree nodes number + remaining nodes number * balanced tree height + numbre of leaves at a height+1 * 2
	//this->remainingBalancedTreeCaseValue = n + (N - n) * floor(log2(N - n)) + ((N - n) - pow(2, floor(log2(N - n))) * 2);

	//todo: improve the Sackin index for the case of an optimally balanced tree
	this->remainingBalancedTreeCaseValue = (N - n) * floor(log2(N - n)) + ((N - n) - pow(2, floor(log2(N - n))) * 2);

	// sackin index value growth after attaching current tree to caterpillar at N - n vertices
	// equals sackin index for caterpillar without one leg at (N - n) vertices + caterpillar length * n 
	this->remainingCaterpillarCaseValue = (((N - n) * (N - n) + (N - n)) / 2) + (N - n) * n;
}

Tree* Tree::Equal(Parameters param, ProgressCounter* pc)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine eng1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	Tree *tree = new Tree(param);

	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + 1;

	int n = 0;
	// create root and two initial vertices
	tree->root = Node::Add();

	tree->allNodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode, *tmpnode;
	for (int i = 0; i < 2; i++) {
		node = Node::Add(label[n++], tree->root);

		tree->allNodes.push_back(node);
		Node::join(tree->root, node);
		if (param.weighted)
		{
 
		}
		tree->allEdges.push_back(Edge::Add(tree->root, node));

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
		edge = tree->allEdges[range_0_INTMAX_unif_int_distr(eng1) % tree->allEdges.size()];
		// always take last edge (only for debuging)
		//edge = tree->edges[tree->edges.size() - 1];
		// always take first edge (only for debuging)
		//edge = tree->edges[0];

		// add bifurcation with 1-P probability
		if (param.P < static_cast<float>(range_0_INTMAX_unif_int_distr(eng1)) / static_cast<float>(INT_MAX)) {

			// remove taken edge
			Edge::erase(tree->allEdges, edge);
			Node::erase(edge->parent->edges, edge->child);
			Node::erase(edge->child->edges, edge->parent);

			// add new internal vertex
			inode = Node::Add();

			tree->allNodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(edge->parent, inode);
			tree->allEdges.push_back(Edge::Add(edge->parent, inode));

			Node::join(inode, edge->child);
			tree->allEdges.push_back(Edge::Add(inode, edge->child));

		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->parent;
		}

		//cutting off unnecessary calculations
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
		tree->allEdges.push_back(Edge::Add(inode, node));

		tree->allNodes.push_back(node);

		if (pc) pc->updateProgress(n);
	}

	// shrink root verticle
	if (!rooted) {
		if (tree->root->edges.size() == 2) {
			// take two root neighbours
			node = tree->root->edges[0];
			inode = tree->root->edges[1];
			// remove root with two incident edges
			Edge::erase(tree->allEdges, tree->root);
			Node::separate(node, tree->root);
			Node::separate(inode, tree->root);
			// add new edge between root neighbours
			Node::join(node, inode);
			tree->allEdges.push_back(Edge::Add(node, inode));

			// pointer to root must be set to internal vertex
			tree->root = inode;
		}
	}

	delete[] label;
	return tree;
}

Tree*Tree::Yule(Parameters param, ProgressCounter* pc)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::default_random_engine eng1(rd());
	std::uniform_int_distribution<int> range_0_INTMAX_unif_int_distr(0, INT_MAX);
	std::uniform_real_distribution<float> range_0_1_unif_float_distr(0, 1);
	Tree *tree = new Tree(param);

	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + param.startingLabel;

	// permutate labels
	for (int i = N - 1; i >= 1; i--)
	{
		int index = range_0_INTMAX_unif_int_distr(eng1) % (i + 1);
		swap(label[index], label[i]);
	}

	int n = 0;
	// create root and two initial vertices
	tree->root = Node::Add();
	tree->allNodes.push_back(tree->root);
	// create two initial vertices and join them with root
	Node *node, *inode;
	for (int i = 0; i < 2; i++) {
		node = Node::Add(label[n++]);
		tree->allNodes.push_back(node);
		Node::join(tree->root, node);
		tree->allEdges.push_back(Edge::Add(tree->root, node));
	}

	while (true)
	{
		if (n == N)
			break;

		// take random pending edge (at least one vertex with non-zero index)
		Edge *edge;
		do
		{
			edge = tree->allEdges[range_0_INTMAX_unif_int_distr(eng1) % tree->allEdges.size()];
			// always take last edge (only for debuging)
			//edge = tree->edges[tree->edges.size() - 1];
			// always take first edge (only for debuging)
			//edge = tree->edges[0];
		} while (edge->pendant() == false && !(n == N && rooted));

		// add bifurcation with 1-P probability
		if (param.P < static_cast<float>(range_0_1_unif_float_distr(eng1))) {

			// remove taken edge
			Edge::erase(tree->allEdges, edge);
			Node::erase(edge->parent->edges, edge->child);
			Node::erase(edge->child->edges, edge->parent);

			// add new internal vertex
			inode = Node::Add();
			tree->allNodes.push_back(inode);

			// add two edges betwen new internal vertex and removed edge neighbours
			Node::join(inode, edge->parent);
			tree->allEdges.push_back(Edge::Add(inode, edge->parent));
			Node::join(inode, edge->child);
			tree->allEdges.push_back(Edge::Add(inode, edge->child));

		}
		// otherwise, multifurcation will be added
		else {
			inode = edge->parent;
		}

		// create new vertex
		node = Node::Add(label[n++]);
		Node::join(inode, node);
		tree->allEdges.push_back(Edge::Add(inode, node));
		tree->allNodes.push_back(node);

		if (pc) pc->updateProgress(n);
	}

	// shrink root verticle
	if (!rooted) {
		if (tree->root->edges.size() == 2) {
			// take two root neighbours
			node = tree->root->edges[0];
			inode = tree->root->edges[1];
			// remove root with two incident edges
			Edge::erase(tree->allEdges, tree->root);
			Node::separate(node, tree->root);
			Node::separate(inode, tree->root);
			// add new edge between root neighbours
			Node::join(node, inode);
			tree->allEdges.push_back(Edge::Add(node, inode));
			// pointer to root must be set to internal vertex
			tree->root = inode;
		}
	}

	delete[] label;
	return tree;
}

void Tree::All(Parameters param, ProgressCounter* pc)
{
	Tree *tree = new Tree(param);
	int *label = new int[N];
	for (int i = 0; i < N; i++)
		label[i] = i + param.startingLabel;

	int n = 0;
	// create two initial vertex and join them
	tree->root = new Node(label[n++]);
	tree->allNodes.push_back(tree->root);
	Node *node = new Node(label[n++]);
	tree->allNodes.push_back(node);
	Node::join(tree->root, node);
	tree->allEdges.push_back(new Edge(tree->root, node));

	tree->Explode(n, N, tree, *param.file, label, pc);
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
			for (int i = 0; i < allNodes.size(); i++) {
				// for all internal nodes
				if (allNodes[i]->label == 0)
				{
					if (pc)
					{
						pc->nextTreeCounted();
					}
					Tree::Print(allNodes[i], NULL, file, pc);
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
		for (int i = 0; i < allNodes.size(); i++) {
			// for all internal nodes
			if (allNodes[i]->label == 0)
			{
				// create new leaf
				Node *leaf = new Node(label[n]);
				// and join it with internal node
				Node::join(allNodes[i], leaf);
				Edge *second = new Edge(allNodes[i], leaf);
				allEdges.push_back(second);
				allNodes.push_back(leaf);
				if ((generateAllTrees || internalNodesNumber <= internalNodesNumberExpected))
				{
					Explode(n + 1, N, tree, file, label, pc);
				}
				// remove all changes
				allNodes.pop_back();
				allEdges.pop_back();
				delete second;
				Node::erase(allNodes[i]->edges, leaf);
				delete leaf;
			}

		}
	}

	for (int i = 0; i < allEdges.size(); i++)
	{
		Edge *edge = allEdges[i];

		// remove edge
		Edge::erase(allEdges, edge);
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
		allEdges.push_back(left);
		allEdges.push_back(right);
		allNodes.push_back(root);

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
			allEdges.push_back(third);
			allNodes.push_back(leaf);
			if ((generateAllTrees || internalNodesNumber <= internalNodesNumberExpected))
			{
				Explode(n + 1, N, tree, file, label, pc);
			}
			allNodes.pop_back();
			allEdges.pop_back();
			delete third;
			Node::erase(leaf->edges, root);
			delete leaf;
		}

		// remove all changes
		allNodes.pop_back();
		allEdges.pop_back();
		allEdges.pop_back();
		delete left;
		delete right;

		Node::erase(edge->parent->edges, root);
		Node::erase(edge->child->edges, root);

		delete root;
		root = node;

		internalNodesNumber--;

		Node::join(edge->parent, edge->child);
		allEdges.insert(allEdges.begin() + i, edge);
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
	if (rooted) {
		Tree::CountSackinIndex(node, NULL, sackinInd);
	}
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
		if (pc) pc->nextTreeRejected();
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
	if (!node->weight.empty())
	{
		file << ':' << node->weight;
	}
}

void Tree::PrintDebug(Tree* tree, ostream& file)
{
	file << "Edges (parent-child):" << endl;
	for (int i = 0; i < tree->allEdges.size(); i++) {
		file << tree->allEdges[i]->parent->label;
		file << '-';
		file << tree->allEdges[i]->child->label << endl;
	}
	file << "Nodes (index. label-parent,(child_0,..)):" << endl;
	for (int i = 0; i < tree->allNodes.size(); i++) {
		file << i << ". ";
		if (tree->allNodes[i]) {
			file << tree->allNodes[i]->label;
			file << '-';
			if (tree->allNodes[i]->parent) {
				file << tree->allNodes[i]->parent->label;
			}
			file << '(';
			bool print = false;
			for (int j = 0; j < tree->allNodes[i]->edges.size(); j++) {
				if (print) {
					file << ',';
				}
				else {
					print = true;
				}
				file << tree->allNodes[i]->edges[j]->label;
			}
			file << ')';
		}
		file << endl;
	}
}

void Tree::ClearAndDelete()
{
	while (!allNodes.empty())
	{
		Node *node = allNodes.back();
		allNodes.pop_back();
		node->edges.clear();
	}
	allEdges.clear();
	Node::index = 0;
	Edge::index = 0;
	delete this;
}

void Tree::Delete()
{
	while (!allEdges.empty())
	{
		Edge *edge = allEdges.back();
		allEdges.pop_back();
		delete edge;
	}

	while (!allNodes.empty())
	{
		Node *node = allNodes.back();
		allNodes.pop_back();
		delete node;
	}

	delete this;
}

Tree::~Tree() { count--; }