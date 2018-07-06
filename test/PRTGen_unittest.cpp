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

TEST(ProgressCounterTests, Returns4asNumberOfAllRootedTreesWith3Leaves) {
	ProgressCounter pc(3, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 4);
}

TEST(ProgressCounterTests, Returns26asNumberOfAllRootedTreesWith4Leaves) {
	ProgressCounter pc(4, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 26);
}

TEST(ProgressCounterTests, Returns236asNumberOfAllRootedTreesWith5Leaves) {
	ProgressCounter pc(5, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 236);
}

TEST(ProgressCounterTests, Returns2752asNumberOfAllRootedTreesWith6Leaves) {
	ProgressCounter pc(6, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 2752);
}

TEST(ProgressCounterTests, Returns282137824asNumberOfAllRootedTreesWith10Leaves) {
	ProgressCounter pc(10, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 282137824);
}

TEST(ProgressCounterTests, Returns6939897876asNumberOfAllRootedTreesWith11Leaves) {
	ProgressCounter pc(11, 0, true, false);
	EXPECT_EQ(pc.get_trees_number(), 6939897856);
}

TEST(ProgressCounterTests, Returns1asNumberOfAllUnootedTreesWith3Leaves) {
	ProgressCounter pc(3, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 1);
}

TEST(ProgressCounterTests, Returns4asNumberOfAllUnootedTreesWith4Leaves) {
	ProgressCounter pc(4, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 4);
}

TEST(ProgressCounterTests, Returns26asNumberOfAllUnootedTreesWith5Leaves) {
	ProgressCounter pc(5, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 26);
}

TEST(ProgressCounterTests, Returns236asNumberOfAllUnootedTreesWith6Leaves) {
	ProgressCounter pc(6, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 236);
}

TEST(ProgressCounterTests, Returns12818912asNumberOfAllUnootedTreesWith10Leaves) {
	ProgressCounter pc(10, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 12818912);
}

TEST(ProgressCounterTests, Returns282137824asNumberOfAllUnootedTreesWith11Leaves) {
	ProgressCounter pc(11, 0, false, false);
	EXPECT_EQ(pc.get_trees_number(), 282137824);
}