#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

PHONY : all all-mic doxy deploy-mic

include Makefile.conf

EXECS = map-reduce seq farm profile
MIC_EXECS = map-reduce-mic seq-mic farm-mic profile-mic

.SUFFIXES:

%.o: %.cpp %.hpp src/graph-search.hpp
	$(CXX) $< $(CXXFLAGS) -I$(FF_ROOT) -c -o $@ $(LIBS)
	
%.o: %.cpp src/graph-search.hpp
	$(CXX) $< $(CXXFLAGS) -I$(FF_ROOT) -c -o $@ $(LIBS)

src/%.o: src/%.cpp src/%.hpp src/graph-search.hpp
	$(CXX) $< $(CXXFLAGS) -I$(FF_ROOT) -c -o $@ $(LIBS)

src/mic/%.o: src/%.cpp src/%.hpp src/graph-search.hpp
	$(CXX) $< $(CXXFLAGS) -I$(FF_ROOT) -c -o $@

src/mic/%-mic.o: src/%.cpp src/graph-search.hpp
	$(ICC) $< $(ICCFLAGS) -I$(FF_ROOT) -c -o $@

$(EXECS): %: src/utils.o src/nodes.o src/%.o
	$(CXX) $(CXXFLAGS) $^ -I$(FF_ROOT) -o $(BUILD_DIR)/$@ $(LIBS)

$(MIC_EXECS): %: src/mic/utils.o src/mic/nodes.o src/mic/%.o
	$(ICC) $(ICCFLAGS) $^ -I$(FF_ROOT) -o $(BUILD_DIR)/mic/$(subst -mic,,$@) $(LIBS)

all: $(EXECS)
all-mic: $(MIC_EXECS)

snap-test: utils
	$(CXX) $(CXXFLAGS) -o build/snap_test src/snap_test.cpp src/utils.o ../snap/snap-core/Snap.o -I../snap/snap-core -I../snap/glib-core -lrt

deploy-mic:
	@ssh mic0 "mkdir -p build/mic"
	@for i in $(ls build/mic) \
	    scp build/mic/$$i mic0:build/mic/$$i \
	 done
	@scp test/simple-scalability-test.sh mic0:test/

doxy:
	doxygen docs/doxy.conf

clean:
	rm -rf build/
	mkdir -p build/mic
	rm -f src/*.o
	rm -f src/mic/*.o

# TESTING TARGETS

test-grain:
	@bash test/scalability-test.sh $(DATASET) $(NEEDLES_FILE) host > TEST_GRAIN_RESULTS_$$(date "+%F-%H-%M") &
	@echo -e "Test started. Results in the TEST_GRAIN_RESULTS file"

test-grain-mic:
	@ssh mic0 "bash test/scalability-test.sh $(DATASET) $(NEEDLES_FILE) mic" > TEST_GRAIN_MIC_RESULTS_$$(date "+%F-%H-%M") &
	@echo -e "Test started. Results in the TEST_GRAIN_MIC_RESULTS file"

test-scale:
	@bash test/simple-scalability-test.sh $(DATASET) $(NEEDLES_FILE) host > TEST_SCALE_RESULTS_$$(date "+%F-%H-%M") &
	@echo -e "Test started. Results in the TEST_SCALE_RESULTS file"
	
test-scale-mic: 
	@ssh mic0 "bash test/simple-scalability-test.sh $(DATASET) $(NEEDLES_FILE) mic" >TEST_SCALE_MIC_RESULTS_$$(date "+%F-%H-%M") &
	@echo -e "Test started. Results in the TEST_SCALE_MIC_RESULTS file"

test-correctness:
	@./test/correctness.sh data/fake-graph.txt{,.needles,.result}