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
	float P = 0.0;
	bool rooted = true;
	bool binary = true;
	Model model = ALL;
	TreeRearrangement treeRear = UNDEFINED;
	ostream *file = NULL;
	char* tmpoptarg = NULL;
};