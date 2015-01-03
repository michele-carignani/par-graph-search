
PHONY : tests-simple-par-mic 

CXX = icc
C_VARS = -g -std=c++11 -O3
FF_ROOT = ../mc-fastflow-code
PROFILING_FLAGS = -profile-functions

TARGETS = sequential-search

sequential-search: src/sequential-search.cpp
	icc $(C_VARS) src/$@.cpp -o build/$@

nodes:
	icc $(C_VARS) src/$@.cpp -c -I$(FF_ROOT) -lpthread

farm pipe-farm: src/nodes.o
	icc $(C_VARS) src/$@.cpp src/nodes.o -I$(FF_ROOT) -o build/$@

simple-par: src/simple-pipe-farm.cpp
	$(CXX) $(C_VARS) -I$(FF_ROOT) $(PROFILING_FLAGS) src/simple-pipe-farm.cpp -o build/simple-par -lpthread

simple-par-mic: src/simple-pipe-farm.cpp
	$(CXX) -mmic $(C_VARS)  -DNO_DEFAULT_MAPPING -I$(FF_ROOT) src/simple-pipe-farm.cpp -o build/simple-par -lpthread


# TESTS

DATASET = web-BerkStan.txt
DATASET_NEEDLES = 255680 254913 450698 438238

tests-simple-par:
	make simple-par
	time build/simple-par data/$(DATASET) 2 $(DATASET_NEEDLES)


tests-simple-par-mic:
	make simple-par-mic
	scp build/simple-par-mic mic0:
	num1=1 ; while [[ $$num1 -le 128 ]] ; do \
		echo "Test $(DATASET) with par deg " $$num1; \
		ssh mic0 'time ./simple-par-mic ./$(DATASET) $$num1  $(DATASET_NEEDLES) > /dev/null' ; \
		((num1 = num1 * 2)) ; \
	done	
