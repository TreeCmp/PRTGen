#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

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
		
		static void erase(deque<Edge*>& edges, Edge *a)
		{		
			for(int i = 0; i < edges.size(); i++)
				if(edges[i] == a)
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
	
		Tree() { count++; }
	
		static Tree* Equal(int N, bool rooted)
		{
			Tree *tree = new Tree();
			
			int *label = new int[N];
			for(int i = 0; i < N; i++)
				label[i] = i + 1;
			
			int n = 0;
			tree->root = new Node(label[n++]);
			tree->nodes.push_back(tree->root);
			
			Node *node = new Node(label[n++]);
			Node::join(tree->root, node);
			tree->edges.push_back(new Edge(tree->root, node));
			tree->nodes.push_back(node);
			
			while(true)
			{
				if(n == N && !rooted)
					break;
				
				Edge *edge = tree->edges[rand() % tree->edges.size()];
				
				Edge::erase(tree->edges, edge);
				Node::erase(edge->left->edges, edge->right);
				Node::erase(edge->right->edges, edge->left);
				
				tree->root = new Node();
				Node::join(tree->root, edge->left);
				tree->edges.push_back(new Edge(tree->root, edge->left));
				Node::join(tree->root, edge->right);
				tree->edges.push_back(new Edge(tree->root, edge->right));
				tree->nodes.push_back(tree->root);
				
				delete edge;
				
				if(n == N && rooted)
					break;
				
				node = new Node(label[n++]);
				Node::join(tree->root, node);
				tree->edges.push_back(new Edge(tree->root, node));
				tree->nodes.push_back(node);
			}
			delete[] label;
			return tree;
		}
		
		static Tree* Yule(int N, bool rooted)
		{
			Tree *tree = new Tree();
			
			int *label = new int[N];
			//stara wersja
            //for(int i = 0; i < N; i++)
			//	label[i] = i + 1;
			//for(int i = N - 1; i >= 1; i--)
			//{
			//	int index = rand() % i, temp = label[index];
			//	label[index] = label[i], label[i] = temp;
			//}
			
			//moja poprawka
            for(int i = 0; i < N; i++)
				label[i] = i + 1;
			for(int i = N - 1; i >= 1; i--)
			{
				int index = rand() % (i+1), temp = label[index];
				label[index] = label[i], label[i] = temp;
			}
			
			int n = 0;
			tree->root = new Node(label[n++]);
			tree->nodes.push_back(tree->root);
			Node *node = new Node(label[n++]);
			Node::join(tree->root, node);
			tree->edges.push_back(new Edge(tree->root, node));
			tree->nodes.push_back(node);
			
			while(true)
			{
				if(n == N && !rooted)
					break;
				
				Edge *edge;
				do
				{
					edge = tree->edges[rand() % tree->edges.size()];
				}
				while(edge->pendant() == false && !(n == N && rooted));
				
				Edge::erase(tree->edges, edge);
				Node::erase(edge->left->edges, edge->right);
				Node::erase(edge->right->edges, edge->left);
				
				tree->root = new Node();
				Node::join(tree->root, edge->left);
				tree->edges.push_back(new Edge(tree->root, edge->left));
				Node::join(tree->root, edge->right);
				tree->edges.push_back(new Edge(tree->root, edge->right));
				tree->nodes.push_back(tree->root);
				
				delete edge;
				
				if(n == N && rooted)
					break;
				
				node = new Node(label[n++]);
				Node::join(tree->root, node);
				tree->edges.push_back(new Edge(tree->root, node));
				tree->nodes.push_back(node);
			}
			delete[] label;
			return tree;
		}
		
		static void All(int N, bool rooted, ostream& file)
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
			tree->root = new Node(label[n++]);
			tree->nodes.push_back(tree->root);
			
			Node *node = new Node(label[n++]);
			Node::join(tree->root, node);
			tree->edges.push_back(new Edge(tree->root, node));
			tree->nodes.push_back(node);
			
			tree->Explode(n, N, rooted, file, label);
			
			delete [] label;
			tree->Delete();
		}
		
		void Explode(int n, int N, bool rooted, ostream& file, int *label)
		{
			if(n == N && !rooted)
			{
				Print(root, NULL, file);
				file << ";" <<endl;
				return;
			}
			
			for(int i = 0; i < edges.size(); i++)
			{
				Edge *edge = edges[i];
				
				Edge::erase(edges, edge);
				Node::erase(edge->left->edges, edge->right);
				Node::erase(edge->right->edges, edge->left);
				
				Node *node = root;
				root = new Node();
				
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
					Node *leaf = new Node(label[n]);
					Node::join(root, leaf);
					Edge *third = new Edge(root, leaf);
					edges.push_back(third);
					nodes.push_back(leaf);
					
					Explode(n + 1, N, rooted, file, label);
					
					nodes.pop_back();
					edges.pop_back();
					delete third;
					Node::erase(leaf->edges, root);
					delete leaf;
				}
				
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
		TreeGenerator(int N, Model model, int M, bool rooted, ostream& file)
		{
			srand(time(NULL));
			switch(model)
			{
				case EQUAL:
					//file << M << endl;
					for(int i = 0; i < M; i++)
					{
						Tree *tree = Tree::Equal(N, rooted);
						Tree::Print(tree->root, NULL, file);
						file << ";" <<endl;
						tree->Delete();
					}
					break;
				case YULE:
					//file << M << endl;
					for(int i = 0; i < M; i++)
					{
						Tree *tree = Tree::Yule(N, rooted);
						Tree::Print(tree->root, NULL, file);
						file << ";"<< endl;
						tree->Delete();
					}
					break;
				case ALL:
					Tree::All(N, rooted, file);
					break;
			}
		
			/*
			cout << "Tree balance: " << Tree::count << endl;
			cout << "Edge balance: " << Edge::count << endl;
			cout << "Node balance: " << Node::count << endl;
			*/
		}
};

int main(int count, char **value)
{
	int N, M;
	bool rooted = true;
	Model model = ALL;
	ofstream *file = NULL;

	int option;
	while ((option = getopt (count, value, "n:e:y:ruf:")) != -1)
		switch(option)
		{
			case 'n':
				N = atoi(optarg);
				break;
			case 'e':
				model = EQUAL;
				M = atoi(optarg);
				break;
			case 'y':
				model = YULE;
				M = atoi(optarg);
				break;
			case 'r':
				rooted = true;
				break;
			case 'u':
				rooted = false;
				break;
			case 'f':
				file = new ofstream(optarg);
				if(file == NULL)
				{
					cout << "Problem z otwarciem pliku: " << optarg << endl;
					return 0;
				}
				break;
			default:
				cout << "Konfiguracja: ./PTGen [-n N] [-ey M] [-ru] [-f X]" << endl;
				return 0;
		}
	
	if(M < 1 && model != ALL)
		cout << "Wymagane M >= 1" << endl;
	else if(N < 3)
		cout << "Wymagane N >= 3" << endl;
	else if(file == NULL)
		new TreeGenerator(N, model, M, rooted, cout);
	else
	{
		new TreeGenerator(N, model, M, rooted, *file);	
		file->close();
	}
	
	return 0;
}
