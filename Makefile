#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

PHONY : all all-mic doxy

include Makefile.conf

# BUILD TARGETS

utils: src/utils.hpp src/utils.cpp src/graph-search.hpp
	$(CXX) $(CXXFLAGS) src/$@.cpp -c -I$(FF_ROOT) -o src/$@.o

nodes: src/nodes.hpp src/nodes.cpp src/graph-search.hpp
	$(CXX) $(CXXFLAGS) src/$@.cpp -c -I$(FF_ROOT) -o src/$@.o
	
seq: utils src/seq.cpp
	$(CXX) $(CXXFLAGS) src/$@.cpp src/utils.o -o build/$@ -lrt

seq-no-io: utils src/seq-no-io.cpp
	$(CXX) $(CXXFLAGS) src/$@.cpp src/utils.o -o build/$@ -lrt

farm : nodes utils src/farm.cpp
	$(CXX) $(CXXFLAGS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

map : utils src/map.cpp
	$(CXX) $(CXXFLAGS) src/$@.cpp src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

farm-no-io :nodes utils src/farm-no-io.cpp
	$(CXX) $(CXXFLAGS) src/$@.cpp src/nodes.o src/utils.o -I$(FF_ROOT) -o build/$@ -lpthread -lrt

# BUILD TARGETS FOR MIC (XEON PHI)

utils-mic : src/utils.hpp src/utils.cpp src/graph-search.hpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) -c src/$(subst -mic,,$@).cpp -o src/mic/$(subst -mic,,$@).o

nodes-mic: src/nodes.hpp src/nodes.cpp src/graph-search.hpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) -c src/$(subst -mic,,$@).cpp -o src/mic/$(subst -mic,,$@).o

seq-mic: utils-mic src/seq.cpp
	$(ICC) $(I_VARS) src/$(subst -mic,,$@).cpp src/mic/utils.o -o build/mic/$(subst -mic,,$@) -lrt

seq-no-io-mic: utils-mic src/seq-no-io.cpp
	$(ICC) $(I_VARS) src/$(subst -mic,,$@).cpp src/mic/utils.o -o build/mic/$(subst -mic,,$@) -lrt

farm-mic : nodes-mic utils-mic src/farm.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o src/mic/nodes.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt
	
farm-no-io-mic : nodes-mic utils-mic src/farm-no-io.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o src/mic/nodes.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt

map-mic: utils-mic src/map.cpp
	$(ICC) $(I_VARS) -I$(FF_ROOT) src/$(subst -mic,,$@).cpp src/mic/utils.o -o build/mic/$(subst -mic,,$@) -lpthread -lrt

all: seq seq-no-io farm map farm-no-io profile

all-mic: seq-mic seq-no-io-mic farm-mic map-mic farm-no-io-mic profile-mic

profile: utils
	$(CXX) $(CXXFLAGS) src/profile.cpp src/utils.o -o build/profile -lrt

profile-mic: utils-mic
	$(ICC) $(I_VARS) src/profile.cpp src/mic/utils.o -o build/mic/profile -lrt

# CXXFLAGS = -std=c++11 -O3 -DNDEBUG
snap-test: utils
	$(CXX) $(CXXFLAGS) -o build/snap_test src/snap_test.cpp src/utils.o ../snap/snap-core/Snap.o -I../snap/snap-core -I../snap/glib-core -lrt

deploy-mic :
	@for i in $(ls build/mic) ; scp build/mic/$i mic0:build/mic/$i ; done
	@scp test/simple-scalability-test.sh mic0:test/

doxy:
	doxygen docs/doxy.conf

# TESTING TARGETS

DATASET = web-BerkStan.txt
NEEDLES_FILE = web-BerkStan.txt.long.needles

MIC_PREFIX_DIR=par-grap-searh

test-scale:
	@echo -e "\nBuilding executables.."
	@make all

	@echo -e "\nRunning tests"
	@bash tests/scalability-test.sh data/$(DATASET) data/$(NEEDLES_FILE) > TEST_SCALE_RESULTS &

test-mic:
	@echo -e "\nBuilding executables.."
	@make all-mic

	@make deploy

	@echo -e "\nRunning test!"
	@ssh mic0 "bash test/simple-scalability-test.sh data/$(DATASET) data/$(NEEDLES_FILE)" >TEST_MIC_RESULTS_$(date "+%F-%H:%M") &
	@echo -e "Test running. Results in the TEST_SCALE_RESULTS file"

test-correctness:
	@./test/correctness.sh data/	fake-graph.txt{,.needles,.result}