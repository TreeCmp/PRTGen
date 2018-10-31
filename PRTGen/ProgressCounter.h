#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>

class ProgressCounter
{
	unsigned long long trees_number, trees_counted;
	int nodes_number;
	double calcPassed;
	const int interval_percent = 10;
	time_t clock_begin, clock_actual;
	double elapsed_secs = 0.0;
	double prev_elapsed_secs = 0;
	const double interval_secs = 10.0;
	bool rooted, binary;

public:

	ProgressCounter(int N, int M, bool R, bool B, int P);
	unsigned long long get_trees_number();
	void nextTreeCounted();
	void showDateNadTimeNow();
	void updateProgress(int n = 0);
	void finishCalsc();
	void resetElapsed_secs();
};