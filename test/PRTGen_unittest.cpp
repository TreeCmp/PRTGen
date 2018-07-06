// This header file defines the public API for Google Test.
//It should be included by any test program that uses Google Test.
#include "gtest/gtest.h"

/* TEST(test_case_name, test_name) is a predefined macro.
These are ordinary C++ functions that don't return a value.
In this function, along with any valid C++ statements you want to include,
use the various Google Test assertions to check values.
The test's result is determined by the assertions;
if any assertion in the test fails (either fatally or non-fatally),
or if the test crashes, the entire test fails. Otherwise, it succeeds.
*/

#include <tchar.h>
#include "../XGetopt/XGetopt.h"
#include "../PRTGen/ProgressCounter.cpp"

using namespace std;

TEST(SimpleTests, AlwaysReturnsTrue) {
	EXPECT_TRUE(true);
}

TEST(ProgressCounterTests, Returns3asNumberOfBinaryRootedTreesWith3Leaves) {
	ProgressCounter pc(3, 0, true, true);
	EXPECT_EQ(pc.get_trees_number(), 3);
}


