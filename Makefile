
CXX = icc
C_VARS = -g -std=c++11
FF_ROOT = ../mc-fastflow-code

seq: src/sequential-search.cpp
	icc $(C_VARS) src/sequential-search.cpp -o build/seq-graph-search

par: src/pipe-farm-search.cpp
	icc $(C_VARS) src/pipe-farm-search -i$(FF_ROOT) -o build/par-graph-search
