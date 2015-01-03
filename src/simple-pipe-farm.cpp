#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <istream>
#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <vector>
//#include "graph-search.hpp"

using namespace ff;
using namespace std;

ifstream graph_file;
ifstream needles_file;
list<string> needles;

unordered_multimap<int, int> results;

// unordered_map<int, int> results;

int main(int argc, char** argv){
    
    /* Initialization */
    
    cout << "Workers num: " << wn << ", Granularity: " << granularity << " \n";

    ff_pipe<int> pipe(new Emitter(granularity), new ff_farm<>(workers));
    // pipe.wrap_around();
    pipe.cleanup_nodes();
    if(pipe.run_and_wait_end()<0) error("running pipe");

    graph_file.close();

    for(auto& x: results){
        cout << x.first << ": " << x.second << "\n";
    } 

    return 0;
}
