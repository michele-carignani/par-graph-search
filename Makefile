#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

PHONY : all all-mic

CXX = g++
C_VARS = -g -std=c++11 -O3 # -DDEBUG

ICC = icc
I_VARS = -mmic -g -std=c++11 -O3 -DNO_DEFAULT_MAPPING

FF_ROOT = ../../mc-fastflow-code
PROFILING_FLAGS = -profile-functions

seq: utils
	$(CXX) $(C_VARS) src/$@.cpp src/utils.o -o build/$@

utils nodes:
	$(CXX) $(C_VARS) src/$@.cpp -c -I$(FF_ROOT) -o src/$@.o

farm pipe-farm map: nodes utils
	$(CXX) $(C_VARS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread

seq-mic: utils-mic
	$(ICC) $(I_VARS) src/$@.cpp src/utils-mic.o -o build/$@-mic

utils-mic nodes-mic:
	$(ICC) $(I_VARS) -I$(FF_ROOT) -c src/$@.cpp -o src/@-mic.o

farm-mic pipe-farm-mic map-mic: nodes-mic utils-mic
	$(IXX) $(I_VARS) -I$(FF_ROOT) src/@.cpp src/utils-mic.o src/nodes-mic.o -o build/@-mic -lpthread

all: seq farm pipe-farm map 

all-mic: seq-mic farm-mic pipe-farm-mic map-mic

# TESTS

DATASET = berk.txt
NEEDLES_FILE = web-BerkStan.txt.needles
DATASET_NEEDLES = 255680 254913 450698 438238

test-scale:
	make all
	bash test/scalability-test.sh data/$(DATASET) data/$(NEEDLES_FILE)

tests-simple-par:
	make simple-par
	time build/simple-par data/$(DATASET) data/$(DATASET_NEEDLES)

tests-simple-par-mic:
	make simple-par-mic
	scp build/simple-par-mic mic0:
	num1=1 ; while [[ $$num1 -le 128 ]] ; do \
		echo "Test $(DATASET) with par deg " $$num1; \
		ssh mic0 'time ./simple-par-mic ./$(DATASET) $$num1  $(DATASET_NEEDLES) > /dev/null' ; \
		((num1 = num1 * 2)) ; \
	done