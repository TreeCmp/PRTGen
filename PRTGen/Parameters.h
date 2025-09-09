#pragma once

using namespace std;

enum Model
{
	UNKNOWN, ALL, EQUAL, YULE
};

enum TreeRearrangement
{
	UNDEFINED, NNI, SPR, TBR
};

struct Parameters{	
	int N = 0, M = 0, D = 0;
	float P = 0.0; // multifurcation occurrence probability
	int startingLabel = 1;
	bool rooted = true;
	bool binary = true;
	bool weighted = false;
	int minWeightVal = 0;
	int maxWeightVal = INT32_MAX;
	Model model = ALL;
	TreeRearrangement treeRear = UNDEFINED;
	ostream *file = NULL;
	char* tmpoptarg = NULL;
};