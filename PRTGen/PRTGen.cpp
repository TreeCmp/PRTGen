#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#ifdef _WIN32

#include <tchar.h>
#include "..\XGetopt\XGetopt.h"
#define ATOI_FUNC _wtoi
#define ATOF_FUNC _wtof
#define PTGEN_EXE "PTGen.exe"

#else

#include <unistd.h>
#define ATOI_FUNC atoi
#define ATOF_FUNC atof
#define PTGEN_EXE "./PTGen"

#endif

using namespace std;

class ProgressCounter
{
public:

	int trees_number, trees_counted, nodes_number;
	double prevCalcPassed, calcPassed;
	const int interval_percent = 10;
	clock_t clock_begin, clock_actual;
	double elapsed_secs, prev_elapsed_secs;
	const double interval_secs = 10.0;

	ProgressCounter(int N, int M) {
		clock_begin = clock();
		trees_number = M;
		trees_counted = 0;
		nodes_number = N;
	}

	void nextTreeCounted()
	{
		trees_counted++;
	}

	void updateProgress(int n)
	{
		prev_elapsed_secs = elapsed_secs;
		clock_actual = clock();
		elapsed_secs = double(clock_actual - clock_begin) / CLOCKS_PER_SEC;

		prevCalcPassed = calcPassed;
		calcPassed = 100 * (trees_counted / static_cast<double>(trees_number) + n / static_cast<double>(nodes_number) / static_cast<double>(trees_number));

		if (static_cast<int>(elapsed_secs / interval_secs) != static_cast<int>(prev_elapsed_secs / interval_secs) ||
			static_cast<int>(prevCalcPassed / interval_percent) != static_cast<int>(calcPassed) / interval_percent)
		{
			cout << calcPassed << "% calculations passed" << endl;
		}
	}

	void finishCalsc()
	{
		prev_elapsed_secs = elapsed_secs;
		elapsed_secs = double(clock_actual - clock_begin) / CLOCKS_PER_SEC;
		cout << endl << "Calculation time: " << elapsed_secs << " seconds" << endl;
	}
};

enum Model
{
	ALL, EQUAL, YULE
};

class Node
{
	public:
		int index;
		deque<Node*> edges;
		static int count;
	
		Node() : index(0) { count++; }
		Node(int a) : index(a) { count++; }
		
		static void join(Node *a, Node *b)
		{
			a->edges.push_back(b);
			b->edges.push_back(a);
		}

		static void separate(Node *a, Node *b)
		{
			Node::erase(a->edges, b);
			Node::erase(b->edges, a);
		}
		
		static void erase(deque<Node*>& edges, Node *a)
		{		
			for(int i = 0; i < edges.size(); i++)
				if(edges[i] == a)
				{
					edges.erase(edges.begin() + i);
					return;
				}
		}
		
		~Node()	{ count--; }
};

class Edge
{
	public:
		Node *left, *right;
		static int count;
	
		Edge(Node *a, Node *b) : left(a), right(b) { count++; }

		static void erase(deque<Edge*>& edges, Node *n)
		{
			for (int i = 0; i < edges.size(); i++)
				if (edges[i]->left == n || edges[i]->right == n)
				{
					edges.erase(edges.begin() + i);
					return;
				}
		}

		static void erase(deque<Edge*>& edges, Edge *e)
		{		
			for(int i = 0; i < edges.size(); i++)
				if(edges[i] == e)
				{
					edges.erase(edges.begin() + i);
					return;
				}
		}
		
		bool pendant()
		{
			return (left->index > 0) || (right->index > 0);
		}
		
		~Edge()	{ count--; }
};

class Tree 
{
	public:

		Node *root;
		deque<Edge*> edges;
		deque<Node*> nodes;
		static int count;

		Tree() { 
			count++;
		}
	
		static Tree* Equal(int N, bool rooted, bool binary, float P, ProgressCounter* pc)
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

				pc->updateProgress(n);
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
		
		static Tree* Yule(int N, bool rooted, bool binary, float P, ProgressCounter* pc)
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
			for(int i = N - 1; i >= 1; i--)
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
			
			while(true)
			{
				if(n == N)
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
				}
				while(edge->pendant() == false && !(n == N && rooted));
				
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

				pc->updateProgress(n);
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
		
		static void All(int N, bool rooted, bool binary, ostream& file)
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
			for(int i = 0; i < N; i++)
				label[i] = i + 1;
			
			int n = 0; 
			// create two initial vertex and join them
			tree->root = new Node(label[n++]);
			tree->nodes.push_back(tree->root);
			Node *node = new Node(label[n++]);
			Node::join(tree->root, node);
			tree->edges.push_back(new Edge(tree->root, node));
			tree->nodes.push_back(node);
			

			tree->Explode(n, N, rooted, binary, file, label);
			
			delete [] label;
			tree->Delete();
		}
		
		void Explode(int n, int N, bool rooted, bool binary, ostream& file, int *label)
		{
			if(n == N && (!rooted || !binary))
			{
				Print(root, NULL, file);
				file << ";" <<endl;
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
						Explode(n + 1, N, rooted, binary, file, label);
						// remove all changes
						nodes.pop_back();
						edges.pop_back();
						delete second;
						Node::erase(nodes[i]->edges, leaf);
						delete leaf;
					}

				}
			}

			for(int i = 0; i < edges.size(); i++)
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

				if(n == N && rooted)
				{
					Print(root, NULL, file);
					file << ";" <<endl;
				}
				else
				{
					// create new leaf and join it with new internal node
					Node *leaf = new Node(label[n]);
					Node::join(root, leaf);
					Edge *third = new Edge(root, leaf);
					edges.push_back(third);
					nodes.push_back(leaf);
					
					Explode(n + 1, N, rooted, binary, file, label);
					
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
		
		static void Print(Node* node, Node *parent, ostream& file)
		{
			deque<Node*> edges(node->edges);
			Node::erase(edges, parent);
			
			if(!edges.empty())
			{	
				file << "(";
				for(int i = 0; i < edges.size(); i++)
				{
					Print(edges[i], node, file);
					if(i + 1 == edges.size())
						break;
					file << ",";
				}
				file << ")";
			}
			if(node->index > 0)
				file << node->index;
		}
		
		void Delete()
		{
			while(!edges.empty())
			{
				Edge *edge = edges.back();
				edges.pop_back();
				delete edge;
			}
			
			while(!nodes.empty())
			{
				Node *node = nodes.back();
				nodes.pop_back();
				delete node;
			}
			
			delete this;
		}
		
		~Tree() { count--; }
};
int Node::count = 0, Edge::count = 0, Tree::count = 0;

class TreeGenerator
{
	public:
		TreeGenerator(int N, Model model, int M, bool rooted, bool binary, float P, ostream& file)
		{
			ProgressCounter* pc = NULL;
			if (&file != &cout) pc = new ProgressCounter(N, M);
			switch(model)
			{
				case EQUAL:
					//file << M << endl;
					for(int i = 0; i < M; i++)
					{
						Tree *tree = Tree::Equal(N, rooted, binary, P, pc);
						Tree::Print(tree->root, NULL, file);
						file << ";" <<endl;
						tree->Delete();
						if (pc) pc->nextTreeCounted();
					}
					break;
				case YULE:
					//file << M << endl;
					for(int i = 0; i < M; i++)
					{
						Tree *tree = Tree::Yule(N, rooted, binary, P, pc);
						Tree::Print(tree->root, NULL, file);
						file << ";"<< endl;
						tree->Delete();
						if (pc) pc->nextTreeCounted();
					}
					break;
				case ALL:
					Tree::All(N, rooted, binary, file);
					break;
			}
			if (pc)
			{
				pc->finishCalsc();
				delete pc;
			}
		
			/*
			cout << "Tree balance: " << Tree::count << endl;
			cout << "Edge balance: " << Edge::count << endl;
			cout << "Node balance: " << Node::count << endl;
			*/
		}
};

void printHelp() {
	cout << "Configuration: " << PTGEN_EXE <<" [-n N] [-ey M] [-ru] [-f X]" << endl;
	cout << "-n N - number of leaves (required N >= 3)\n";
	cout << "-e M - M trees with uniform distribution\n";
	cout << "-y M - M trees with Yule's distribution\n";
	cout << "-r - rooted trees\n";
	cout << "-u - unrooted trees\n";
	cout << "-b - binary trees\n";
	cout << "-a P - arbitrary trees with P propablility of multifurcation occurence (required 0 >= P >= 1)\n";
	cout << "-f X - save result to X file (default to console)\n\n";
	cout << "Without -e or -y option generates all possible binary arbitraty N-leaf trees\n";
	cout << "With -a and without -e or -y option generates all possible arbitraty N-leaf trees (P value is required but will be ignored)\n";
	cout << "By default binary rooted trees are generated\n";
}

/*********************************************************************************/
/*                                 MAIN FUNCTION                                 */
/*********************************************************************************/

#ifdef _WIN32
int _tmain(TCHAR count, TCHAR **value)
#else
int main(int count, char **value)
#endif

{
	int N = 0, M = 0;
	float P = 0.0;
	bool rooted = true;
	bool binary = true;
	Model model = ALL;
	ofstream *file = NULL;

	int option;
#ifdef _WIN32
	while ((option = getopt(count, value, L"n:e:y:ruba:f:")) != -1)
#else
	while (option = getopt (count, value, "n:e:y:ruba:f:") != -1)
#endif
		switch(option)
		{
			case 'n':
				N = ATOI_FUNC(optarg);
				break;
			case 'e':
				model = EQUAL;
				M = ATOI_FUNC(optarg);

				break;
			case 'y':
				model = YULE;
				M = ATOI_FUNC(optarg);
				break;
			case 'r':
				rooted = true;
				break;
			case 'u':
				rooted = false;
				break;
			case 'b':
				binary = true;
				break;
			case 'a':
				binary = false;
				P = ATOF_FUNC(optarg);
				break;
			case 'f':
				file = new ofstream(optarg);
				if(file == NULL)
				{
					cout << "Problem opening file: " << optarg << endl;
					return 0;
				}
				break;
			default:
				printHelp();
				return 0;
		}
	
	if (M < 1 && model != ALL)
	{
		cout << "Required M >= 1" << endl;
		printHelp();
	}
	else if (N < 3)
	{
		cout << "Required N >= 3" << endl;
		printHelp();
	}
	else if(file == NULL)
		new TreeGenerator(N, model, M, rooted, binary, P, cout);
	else
	{
		new TreeGenerator(N, model, M, rooted, binary, P, *file);
		file->close();
	}
	
	return 0;
}