#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <random>
#include "ProgressCounter.h"
#include "Tree.h"
#ifdef _WIN32

#include <tchar.h>
#include "../XGetopt/XGetopt.h"
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

enum Model
{
	ALL, EQUAL, YULE
};

int Node::count = 0, Edge::count = 0, Tree::count = 0;
bool Tree::binary = true, Tree::rooted = true;
bool Tree::printIndexes = false;
double Tree::minSackinsIndex = DBL_MIN, Tree::maxSackinsIndex = DBL_MAX;
char Tree::sackin_norm_model = 'n';

class TreeGenerator
{
	public:
		TreeGenerator(int N, Model model, int M, bool rooted, bool binary, float P, ostream& file)
		{
			Tree::N = N;
			ProgressCounter* pc = NULL;
			Tree::CountSum();
			switch(model)
			{
				case EQUAL:
					//file << M << endl;
					if (&file != &cout) pc = new ProgressCounter(N, M, rooted, binary, P);
					for(int i = 0; i < M;)
					{
						Tree *tree = Tree::Equal(N, rooted, binary, P, pc);

						// Print root children count and Sackin index value (only for debuging)
						//cout << "Root has = " << tree->root->siblingNumber << " children" << endl;
						//cout << "Sackin index = " << tree->sackinIndexValue << endl;

						if (tree && Tree::Print(tree->root, NULL, file, pc))
						{
							i++;
							if (pc) pc->nextTreeCounted();
						}
						if (tree)
						{
							tree->Delete();
						}
					
					}
					break;
				case YULE:
					//file << M << endl;					
					if (&file != &cout) pc = new ProgressCounter(N, M, rooted, binary, P);
					for(int i = 0; i < M; )
					{
						Tree *tree = Tree::Yule(N, rooted, binary, P, pc);
						if (Tree::Print(tree->root, NULL, file, pc))
						{
							i++;
							if (pc) pc->nextTreeCounted();
						}
						tree->Delete();
					}
					break;
				case ALL:
					if (!binary)
					{
						if (rooted)
						{
							if (P > N - 1)
							{
								cout << "Value of parameter P must be between 0 and " << N - 1 << " (N - 1) for rooted trees on " << N << " leaves" << endl;
								return;
							}
						}
						else
						{
							if (P > N - 2)
							{
								cout << "Value of parameter P must be between 0 and " << N - 2 << " (N - 2) for unrooted trees on " << N << " leaves" << endl;
								return;
							}
						}
					}
					if (&file != &cout) pc = new ProgressCounter(N, M, rooted, binary, P);
					Tree::All(N, rooted, binary, file, P, pc);
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
	cout << "Configuration: " << PTGEN_EXE <<" [-n N] [-e M|-y M] [-r|-u] [-b|-p P] [-f X]" << endl;
	cout << "-n N - number of leaves (required N >= 3)\n";
	cout << "-e M - M trees with uniform distribution\n";
	cout << "-y M - M trees with Yule's distribution\n";
	cout << "-r - rooted trees\n";
	cout << "-u - unrooted trees\n";
	cout << "-b - binary trees\n";
	cout << "-a P - arbitrary trees with P propablility of multifurcation occurence\n";
	cout << " (required 0 >= P >= 1, Yule or uniform case)\n";
	cout << "-i - print indexes values (e.g. Sackin's index)\n";
	cout << "-sn X Y - include only trees in Sackin's index range (normalized values from 0.0-1.0 scope)\n";
	cout << "-sy X Y - include only trees in Sackin's index range (Yule reference model normalized values)\n";
	cout << "-se X Y - include only trees in Sackin's index range (uniform reference model normalized values)\n";
	cout << "-f X - save result to X file (default to console)\n\n";
	cout << "With -b and without -e or -y option generates all possible binary N-leaf trees.\n";
	cout << "With -a and without -e or -y option generates all possible arbitraty N-leaf\n";
	cout <<	" trees (case with P = 0) or generates all possible arbitraty N-leaf \n";
	cout << " trees with I internal nodes (case with P = I where, for rooted trees:\n";
	cout << " 1 <= I <= N-1, and for unrooted trees: 1 <= I <= N-2).\n\n";
	cout << "By default binary rooted trees are generated.\n";
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
	while ((option = getopt(count, value, L"n:e:y:ruba:is:f:")) != -1)
#else
	while (option = getopt (count, value, "n:e:y:ruba:is:f:") != -1)
#endif
		switch(option)
		{
			case 'n':
				N = ATOI_FUNC(optarg);
				break;
			case 'e':
				model = EQUAL;
				Tree::sackin_norm_model = 'e';
				M = ATOI_FUNC(optarg);
				break;
			case 'y':
				model = YULE;
				Tree::sackin_norm_model = 'y';
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
			case 'i':
				Tree::printIndexes = true;
				break;
			case 's':
				Tree::sackin_norm_model = *optarg;
				if (optind < count && *value[optind] != '-') {
					Tree::minSackinsIndex = ATOF_FUNC(value[optind]);
					optind++;
				}
				if (optind < count && *value[optind] != '-') {
					Tree::maxSackinsIndex = ATOF_FUNC(value[optind]);
					optind++;
				}
				else {
					cout << "-s? option require TWO float numbers \n";
					return 0;
				}
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