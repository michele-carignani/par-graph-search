

include ../Makefile.conf

%.o: %.cpp %.hpp graph-search.hpp
		$(CXX) $< $(CXXFLAGS) -I$(FF_ROOT) -c -DLOL -o $@

