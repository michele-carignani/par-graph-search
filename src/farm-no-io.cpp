/** 
 * \file farm-no-io.cpp
 * \author: michele.carignani@gmail.com
 *
 */

#include <cstdlib>
#include <ff/farm.hpp>
#include "nodes.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

/** List of nodes to look for in the graph. */
list<string> needles;

int main(int argc, char** argv) {
    int nw, g, i;
    char* graph_file_path;
    struct timespec start, end;
    #ifdef IO_TIME
    struct timespec end_io;
    #endif
    vector<char*> edgelist;
    ifstream graph_file;
    char buf[100];

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
    
    graph_file.open(graph_file_path);
    
    i = 1;
    graph_file.getline(buf, 100);
    while(graph_file.gcount() != 0){        
        edgelist.push_back(strdup(buf));
        i++;
        graph_file.getline(buf, 100);
    }

    #ifdef IO_TIME
    clock_gettime(CLOCK_REALTIME, &end_io);
    #endif
    
    EmitterNoIO em (edgelist, g);
    Collector col;

    vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
        ManyLinesWorker* w = new ManyLinesWorker (needles) ;
        
        #ifdef USE_AFFINITY
        w->setAffinity(j * 4);
        #endif
        
        workers.push_back(w);
    }
    
    ff_farm<> graph_search_farm (workers, &em, &col);

    if(graph_search_farm.run_and_wait_end()<0) error("running farm");
        
    clock_gettime(CLOCK_REALTIME, &end);    
    
    #ifdef IO_TIME
    cerr << " : " << elapsed_time_secs(end_io, end);
    #else 
    cerr << elapsed_time_secs(start, end);
    #endif
    return 0;
}

