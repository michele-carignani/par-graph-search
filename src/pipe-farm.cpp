/** \file: pipe-farm.cpp
*   \author: Michele Carignani@gmail.com
*/

#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <istream>
#include <fstream>
#include <string>
#include <iostream>

#include "graph-search.hpp"
#include "nodes.hpp"
#include "utils.hpp"

using namespace ff;
using namespace std;

/** List of nodes to be looked for */
list<string> needles;

int main(int argc, char* argv[]){
    int nw, g;
    char* graph_file_path;
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    get_conf(argc, argv, &graph_file_path, &needles, &nw, &g);
    if(nw <= 0 || g <= 0 ) {
        cout << "Error: n-workers and granularity must be grater than 0, ";
        cout  << nw << " and " << g << "  given.\n";
        exit(1);
    }

    #ifdef DEBUG
    cout << "Workers num: " << nw << ", Granularity: " << g << " \n";
    #endif 

    /* Create workers */
    vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
        ManyLinesWorker* w = new ManyLinesWorker(needles);
        #ifdef USE_AFFINITY
        w->setAffinity(j * 4);
        #endif     
        workers.push_back(w);
    }
    
    ManyLinesEmitter emitter (graph_file_path, g);
    ff_farm<> wfarm (workers);
    wfarm.remove_collector();
    Collector collector;
    
    ff_pipe<int> pipe(&emitter, &wfarm, &collector);
    
    pipe.cleanup_nodes();
    if(pipe.run_and_wait_end()<0) error("running pipe"); 

    clock_gettime(CLOCK_REALTIME, &end);
    cerr << elapsed_time_secs(start, end);

    return 0;
}

