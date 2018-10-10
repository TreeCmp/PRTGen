#include <iostream>
#include <iomanip>
#include <ctime>
#include "ProgressCounter.h"

using namespace std;

//double factorial function
static unsigned long long dfact(int n)
{
	int i, res = 1;
	for (i = n; i >= 1; i -= 2)
	{
		res *= i;
	}
	return res;
}

static unsigned long long T(unsigned long long n, unsigned long long m)
{
	if (n == 1) {
		if (m == 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return m * T(n - 1, m) + (n + m - 2) *  T(n - 1, m - 1);
}

ProgressCounter::ProgressCounter(int N, int M, bool R, bool B, int P) {
	nodes_number = N;
	rooted = R;
	binary = B;
	if (M)
	{
		trees_number = M;
	}
	else if (binary)
	{
		trees_number = dfact(2 * nodes_number - (rooted ? 3 : 5));
	}
	else
	{
		trees_number = 0;
		if (rooted) {
			for (int m = 1; m < nodes_number; m++)
			{
				if (!P || (P == m)) {
					trees_number += T(nodes_number, m);
				}
			}
		}
		else
		{
			for (int m = 1; m < nodes_number - 1; m++)
			{
				if (!P || P == m) {
					trees_number += T( nodes_number - 1, m);
				}
			}
		}
	}

	trees_counted = 0;
	clock_begin = clock();
	showDateNadTimeNow();
	cout << "Start of calculation...please wait..." << endl;
	showDateNadTimeNow();
	cout << 0.0 << "% of " << trees_number << (trees_number > 1 ? " trees" : " tree") << " were generated..." << endl;
}

unsigned long long ProgressCounter::get_trees_number()
{
	return trees_number;
}

void ProgressCounter::nextTreeCounted()
{
	trees_counted++;
}

void ProgressCounter::showDateNadTimeNow() {
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	std::cout << (now->tm_year + 1900) << '-' << setfill('0') << setw(2) << setfill('0') << setw(2) << (now->tm_mon + 1) << '-' << setfill('0') << setw(2) << now->tm_mday << " "
		<< setfill('0') << setw(2) << now->tm_hour << ":" << setfill('0') << setw(2) << now->tm_min << ":" << setfill('0') << setw(2) << now->tm_sec << ": ";
}

void ProgressCounter::updateProgress(int n)
{
	clock_actual = clock();
	elapsed_secs = double(clock_actual - clock_begin) / CLOCKS_PER_SEC;
	calcPassed = 100 * (trees_counted / static_cast<double>(trees_number) + n / static_cast<double>(nodes_number) / static_cast<double>(trees_number));

	if (elapsed_secs - prev_elapsed_secs > interval_secs)
	{
		prev_elapsed_secs = elapsed_secs;
		showDateNadTimeNow();
		cout << setfill('0') << fixed << setprecision(0) << calcPassed << "% of " << trees_number << (trees_number > 1 ? " trees" : " tree") << " were generated..." << endl;
	}
}

void ProgressCounter::finishCalsc()
{
	prev_elapsed_secs = elapsed_secs;
	elapsed_secs = double(clock_actual - clock_begin) / CLOCKS_PER_SEC;
	showDateNadTimeNow();
	cout << setfill('0') << fixed << setprecision(0) << calcPassed << "% of " << trees_number << (trees_number > 1 ? " trees" : " tree") << " were generated..." << endl;
	cout << "generating " << trees_number << (binary ? " binary" : " unary") << (rooted ? " rooted" : " unrooted") << (trees_number > 1 ? " trees" : " tree") << " with " << nodes_number << " leves took " << elapsed_secs << " seconds" << endl;
}

void  ProgressCounter::resetElapsed_secs()
{
	clock_begin = clock();
}