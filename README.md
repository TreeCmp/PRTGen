# PRTGen
Phylogenetic random trees generator

Compilation:

[LINUX]
g++ PRTGen.cpp -o PRTGen

[WINDOWS]
use Visual Studio 2015 .sln file

Run:
[LINUX]
./PRTGen [-n N] [-ey M] [-ru] [-f X]
[WINDOWS]
PRTGen.exe [-n N] [-ey M] [-ru] [-f X]
-n N - number of leaves
-e M - M trees with uniform distribution
-y M - M trees with Yule's distribution
-r - rooted trees
-u - unrooted trees
-f X - save result to X file (default to console)

Without -e or -y option generates all possible N-leaf trees
By default rooted trees are generated