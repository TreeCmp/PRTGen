#pragma once

using namespace std;

enum Model
{
	ALL, EQUAL, YULE
};

struct Parameters{	
	int N = 0, M = 0, D = 0;
	float P = 0.0;
	bool rooted = true;
	bool binary = true;
	Model model = ALL;
	ostream *file = NULL;
	char* tmpoptarg = NULL;
};