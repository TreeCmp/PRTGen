# PRTGen
Phylogenetic random trees generator

## Compilation:

  * [LINUX] g++ PRTGen.cpp -o PRTGen
  * [WINDOWS] use Visual Studio 2015 .sln file

## Run:

  * [LINUX] ./PRTGen [-n N] [-e M|-y M] [-r|-u] [-b|-p P] [-f X]
  * [WINDOWS] PRTGen.exe [-n N] [-e M|-y M] [-r|-u] [-b|-p P] [-f X]
  * -n N - number of leaves
  * -e M - M trees with uniform distribution
  * -y M - M trees with Yule's distribution
  * -r - rooted trees
  * -u - unrooted trees
  * -b - binary trees
  * -a P - arbitrary trees with P propablility of multifurcation occurence (required 0 >= P >= 1, Yule or uniform case)
  * -i - print indexes values (e.g. Sackin's index)
  * -s X Y - include only trees in Sackin's index range (normalized values from 0.0-1.0 scope)
  * -f X - save result to X file (default to console)

With -b and without -e or -y option generates all possible binary arbitraty N-leaf trees.
With -a and without -e or -y option generates all possible arbitraty N-leaf trees (case with P = 0) or generates all possible arbitraty N-leaf trees with I internal nodes (case with P = I where, for rooted trees: 1 <= I <= N-1, and for unrooted trees: 1 <= I <= N-2).

By default binary rooted trees are generated.