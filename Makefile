
C_VARS = -g

seq: src/sequential-search.cpp
	g++ $(C_VARS) src/sequential-search.cpp -o build/seq-graph-search
