/** \file: farm.cpp
*   \author: Michele Carignani michele.carignani@gmail.com
*/
#include <list>
#include <iostream>
#include <fstream>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include "graph-search.hpp"
#include "nodes.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

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
    
    ManyLinesEmitter em (graph_file_path, g);
    Collector col ();

	vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
        ManyLinesWorker* w = new PrinterWorker(&needles);
        
        #ifdef USE_AFFINITY
        w->setAffinity(j * 4);
        #endif
        
        workers.push_back(w);
    }

	ff_farm<> graph_search_farm(workers, em);
	
	graph_search_farm.remove_collector();

	if(graph_search_farm.run_and_wait_end()<0) error("running farm");

	clock_gettime(CLOCK_REALTIME, &end);
	cerr << elapsed_time_secs(start, end);
	// cout << elapsed_time_secs(start,end);
	return 0;
}
