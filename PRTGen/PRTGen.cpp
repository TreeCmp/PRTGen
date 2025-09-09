#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <random>
#include "ProgressCounter.h"
#include "Tree.h"
#include "Parameters.h"
#ifdef _WIN32

#include <tchar.h>
#include "../XGetopt/XGetopt.h"
#define PTGEN_EXE "PTGen.exe"

#else

#include <unistd.h>
#define PTGEN_EXE "./PTGen"

#endif

using namespace std;

int Tree::count = 0;
int Node::count = 0, Node::index = 0;
int Edge::count = 0, Edge::index = 0;
bool Tree::binary = true, Tree::rooted = true;
bool Tree::printIndexes = false;
double Tree::minSackinsIndex = -DBL_MAX, Tree::maxSackinsIndex = DBL_MAX;
char Tree::sackin_norm_model = 'n';
Edge* Edge::edges = NULL;
Node* Node::nodes = NULL;

class TreeGenerator
{
	public:
		TreeGenerator(Parameters param)
		{
			Tree::N = param.N;
			ProgressCounter* pc = NULL;
			Tree::CountSum();
			int edgesCount = 2 * param.N;
			int nodesCount = 2 * param.N;
			switch(param.model)
			{
				case EQUAL:
					//file << M << endl;
					Edge::edges = new Edge[edgesCount];
					Node::nodes = new Node[nodesCount];
					if (param.weighted)
					{
						for (int i = 0; i < nodesCount; i++)
						{
							Node::nodes[i].weight = generateRandomWeight(param.minWeightVal,
								param.maxWeightVal,
								param.useFloatingWeights);
						}
					}
					if (param.file != &cout) pc = new ProgressCounter(param);
					for(int i = 0; i < param.M;)
					{
						Tree *tree = Tree::Equal(param, pc);

						// Print root children count and Sackin index value (only for debuging)
						//cout << "Root has = " << tree->root->siblingNumber << " children" << endl;
						//cout << "Sackin index = " << tree->sackinIndexValue << endl;

						if (tree && Tree::Print(tree->root, NULL, *param.file, pc))
						{
							i++;
							if (param.D > 0) {
								switch (param.treeRear)
								{
								case NNI:
									if (param.binary) {
										//cout << "In order, D trees each distant by (1,2,3...D) " << (param.rooted ? "" : "u") << "spr:";
										for (int j = 0; j < param.D; j++) {
											tree->DoNNI();
											//Tree::PrintDebug(tree, *param.file);
											Tree::Print(tree->root, NULL, *param.file, pc);
											if (pc) pc->nextTreeCounted();
										}
										cout << endl;
									}
									else {
										cout << "For arbitrary trees NNI not implemented yet." << endl;
									}
									break;
								case SPR:
									if (param.binary) {
										//cout << "In order, D trees each distant by (1,2,3...D) " << (param.rooted ? "" : "u") << "spr:";
										for (int j = 0; j < param.D; j++) {
											tree->DoSPR();
											//Tree::PrintDebug(tree, *param.file);
											Tree::Print(tree->root, NULL, *param.file, pc);
											if (pc) pc->nextTreeCounted();
										}
										cout << endl;
									}
									else {
										cout << "For arbitrary trees SPR not implemented yet." << endl;
									}
									cout << endl;
									break;
								case TBR:
									if (param.binary) {
										//cout << "In order, D trees each distant by (1,2,3...D) " << (param.rooted ? "" : "u") << "spr:";
										for (int j = 0; j < param.D; j++) {
											tree->DoTBR();
											//Tree::PrintDebug(tree, *param.file);
											Tree::Print(tree->root, NULL, *param.file, pc);
											if (pc) pc->nextTreeCounted();
										}
										cout << endl;
									}
									else {
										cout << "For arbitrary trees TBR not implemented yet." << endl;
									}
									break;
								default:
									break;
								}
							}
							if (pc) pc->nextTreeCounted();
						}
						if (tree)
						{
							tree->ClearAndDelete();
						}
						else
						{
							if (pc) pc->nextTreeRejected();
						}
					}
					break;
				case YULE:
					Edge::edges = new Edge[3 * param.N];
					Node::nodes = new Node[2 * param.N];
					//file << M << endl;					
					if (param.file != &cout) pc = new ProgressCounter(param);
					for(int i = 0; i < param.M; )
					{
						Tree *tree = Tree::Yule(param, pc);
						if (tree && Tree::Print(tree->root, NULL, *param.file, pc))
						{
							i++;
							if (pc) pc->nextTreeCounted();
						}
						if (tree)
						{
							tree->ClearAndDelete();
						}
						else
						{
							if (pc) pc->nextTreeRejected();
						}
					}
					break;
				case ALL:
					if (!param.binary)
					{
						if (param.rooted)
						{
							if (param.P > param.N - 1)
							{
								cout << "Value of parameter P must be between 0 and " << param.N - 1 << " (N - 1) for rooted trees on " << param.N << " leaves" << endl;
								return;
							}
						}
						else
						{
							if (param.P > param.N - 2)
							{
								cout << "Value of parameter P must be between 0 and " << param.N - 2 << " (N - 2) for unrooted trees on " << param.N << " leaves" << endl;
								return;
							}
						}
					}
					if (param.file != &cout) pc = new ProgressCounter(param);
					Tree::All(param, pc);
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

		char* generateRandomWeight(double minWeightVal,
			double maxWeightVal,
			bool useFloatingWeights)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());

			char* result = new char[64];

			if (useFloatingWeights) {
				std::uniform_real_distribution<double> dist(minWeightVal, maxWeightVal);
				double value = dist(gen);
				std::sprintf(result, "%.4f", value);
			}
			else {
				std::uniform_int_distribution<int> dist(static_cast<int>(minWeightVal), static_cast<int>(maxWeightVal));
				int value = dist(gen);
				std::sprintf(result, "%d", value);
			}

			return result;
		}
};

#include <iostream>
#include <string>
#include <limits>

void parseWeightRange(const char* optarg,
	Parameters& params)
{
	std::string arg(optarg);
	size_t pos = arg.find(':');

	if (pos != std::string::npos) {
		std::string left = arg.substr(0, pos);
		std::string right = arg.substr(pos + 1);

		params.minWeightVal = std::stod(left);
		params.maxWeightVal = std::stod(right);

		if (left.find_first_of(".eE") != std::string::npos ||
			right.find_first_of(".eE") != std::string::npos) {
			params.useFloatingWeights = true;
		}
	}
	else {
		params.minWeightVal = std::stod(arg);
		if (arg.find_first_of(".eE") != std::string::npos) {
			params.useFloatingWeights = true;
		}
	}
}

void printHelp() {
	cout << "Configuration: " << PTGEN_EXE << " [-n N] [-e M|-y M] [-r|-u] [-b|-a P] [-f X] [-w min:max]" << endl;
	cout << "-n N  - number of leaves (required N >= 3)\n";
	cout << "-e M  - generate M trees with uniform distribution\n";
	cout << "-y M  - generate M trees with Yule's distribution\n";
	cout << "-r    - rooted trees\n";
	cout << "-u    - unrooted trees\n";
	cout << "-b    - binary trees\n";
	cout << "-a P  - arbitrary trees with probability P of multifurcation occurrence\n";
	cout << "        (required 0 <= P <= 1, Yule or uniform case)\n";
	cout << "-l    - starting label number >= 1 (default = 1)\n";
	cout << "-i    - print index values (e.g., Sackin's index)\n";
	cout << "-sn X Y - include only trees in Sackin's index range (normalized 0.0-1.0)\n";
	cout << "-sy X Y - include only trees in Sackin's index range (Yule reference model)\n";
	cout << "-se X Y - include only trees in Sackin's index range (uniform reference model)\n";
	cout << "-nni D  - for each tree, generate D trees with one random nni/unni modification each\n";
	cout << "-spr D  - for each tree, generate D trees with one random spr/uspr modification each\n";
	cout << "-f X    - save result to file X (default: console)\n";
	cout << "-w min:max - set branch weights from min to max; if at least one number has a dot,\n";
	cout << "             weights will be floating-point, otherwise integers\n\n";

	cout << "With -b and without -e or -y option, generates all possible binary N-leaf trees.\n";
	cout << "With -a and without -e or -y option, generates all possible arbitrary N-leaf trees.\n";
	cout << "  (case P = 0) or all possible arbitrary N-leaf trees with I internal nodes\n";
	cout << "  (case P = I, where for rooted trees: 1 <= I <= N-1, for unrooted trees: 1 <= I <= N-2).\n\n";
	cout << "By default, binary rooted trees are generated.\n";
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
	Parameters param;
	int option;
	char* token = nullptr;

#ifdef _WIN32
	while ((option = getopt(count, value, "n:e:y:ruba:l:is:f:t:w::")) != -1)
#else
	while (option = getopt(count, value, "n:e:y:ruba:l:is:f:t:w::") != -1)
#endif
	switch(option)
	{
		case 'n':
			param.tmpoptarg = optarg;
			if (*param.tmpoptarg == 'n') {
				param.D = atoi(value[optind]);
				param.treeRear = NNI;
				optind++;
			}
			else
			{
				param.N = atoi(param.tmpoptarg);
			}
			break;
		case 'e':
			param.model = EQUAL;
			Tree::sackin_norm_model = 'e';
			param.M = atoi(optarg);
			break;
		case 'y':
			param.model = YULE;
			Tree::sackin_norm_model = 'y';
			param.M = atoi(optarg);
			break;
		case 'r':
			param.rooted = true;
			break;
		case 'u':
			param.rooted = false;
			break;
		case 'b':
			param.binary = true;
			break;
		case 'a':
			param.binary = false;
			param.P = atof(optarg);
			break;
		case 'l':
			param.startingLabel = atoi(optarg);
			break;
		case 'i':
			Tree::printIndexes = true;
			break;
		case 's':
			param.tmpoptarg = optarg;
			if (*param.tmpoptarg == 'p') {
				param.D = atoi(value[optind]);
				param.treeRear = SPR;
				optind++;
			}
			else {
				Tree::sackin_norm_model = *param.tmpoptarg;
				if (optind < count && *value[optind] != '-') {
					Tree::minSackinsIndex = atof(value[optind]);
					optind++;
				}
				if (optind < count && *value[optind] != '-') {
					Tree::maxSackinsIndex = atof(value[optind]);
					optind++;
				}
				else {
					cout << "-s? option require two float numbers \n";
					return 0;
				}
			}
			break;
		case 'f':
			param.file = new ofstream(optarg);
			if(param.file == NULL)
			{
				cout << "Problem opening file: " << optarg << endl;
				return 0;
			}
			break;
		case 't':
			param.tmpoptarg = optarg;
			if (*param.tmpoptarg == 'b') {
				param.D = atoi(value[optind]);
				param.treeRear = TBR;
				optind++;
			}
			else
			{
				cout << "Wrong argument: t" << optarg << endl;
				return 0;
			}
		case 'w':
			param.weighted = true;
			param.tmpoptarg = optarg;
			parseWeightRange(optarg, param);
			break;
		default:
			printHelp();
			return 0;
	}
	
	if (param.M < 1 && param.model != ALL)
	{
		cout << "Required M >= 1" << endl;
		printHelp();
	}
	else if (param.N < 3)
	{
		cout << "Required N >= 3" << endl;
		printHelp();
	}
	else if (param.file == NULL) {
		param.file = &cout;
		new TreeGenerator(param);
	}
	else
	{
		new TreeGenerator(param);
		//param.file->close();
	}
	
	return 0;
}