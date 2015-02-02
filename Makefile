#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

PHONY : all all-mic doxy

include Makefile.conf

# BUILD TARGETS

utils: src/utils.hpp src/utils.cpp src/graph-search.hpp
	$(CXX) $(C_VARS) src/$@.cpp -c -I$(FF_ROOT) -o src/$@.o

nodes: src/nodes.hpp src/nodes.cpp src/graph-search.hpp
	$(CXX) $(C_VARS) src/$@.cpp -c -I$(FF_ROOT) -o src/$@.o
	
seq: utils src/seq.cpp
	$(CXX) $(C_VARS) src/$@.cpp src/utils.o -o build/$@ -lrt

farm : nodes utils src/farm.cpp
	$(CXX) $(C_VARS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

map : nodes utils src/map.cpp
	$(CXX) $(C_VARS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

farm-no-io :nodes utils src/farm-no-io.cpp
	$(CXX) $(C_VARS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

# BUILD TARGETS FOR MIC (XEON PHI)

utils-mic : src/utils.hpp src/utils.cpp src/graph-search.hpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) -c src/$(subst -mic,,$@).cpp -o src/mic/$(subst -mic,,$@).o

nodes-mic: src/nodes.hpp src/nodes.cpp src/graph-search.hpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) -c src/$(subst -mic,,$@).cpp -o src/mic/$(subst -mic,,$@).o

seq-mic: utils-mic src/seq.cpp
	$(ICC) $(I_VARS) src/$(subst -mic,,$@).cpp src/mic/utils.o -o build/mic/$(subst -mic,,$@) -lrt

farm-mic : nodes-mic utils-mic src/farm.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o src/mic/nodes.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt
	
farm-no-io-mic : nodes-mic utils-mic src/farm-no-io.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o src/mic/nodes.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt

map-mic: utils-mic src/map.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt

all: seq farm map farm-no-io

all-mic: seq-mic farm-mic map-mic farm-no-io-mic

doxy:
	doxygen docs/doxy.conf

# TESTING TARGETS

DATASET = web-BerkStan.txt
NEEDLES_FILE = web-BerkStan.txt.needles

MIC_PREFIX_DIR=par-grap-searh

test-scale:
	@echo -e "\nBuilding executables.."
	@make all

	@echo -e "\nRunning tests"
	@bash tests/scalability-test.sh data/$(DATASET) data/$(NEEDLES_FILE) > TEST_SCALE_RESULTS &

tests-scale-mic:
	@echo -e "\nBuilding executables.."
	@make all-mic

	bash test/prepare-test.sh $(MIC_PREFIX_DIR) $(DATASET) $(NEEDLES_FILE)

	@echo -e "\nRunning test!"
	@ssh mic0 "cd $(MIC_PREFIX_DIR); bash test/scalability-test.sh data/$(DATASET) data/$(NEEDLES_FILE) > TEST_SCALE_RESULTS &"
	@echo -e "Test running. Results in the TEST_SCALE_RESULTS file"
