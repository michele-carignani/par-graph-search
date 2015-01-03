#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <istream>
#include <fstream>
#include <string>
#include <iostream>

#include "graph-search.hpp"
#include "nodes.hpp"

using namespace ff;
using namespace std;

ifstream graph_file;
list<string> needles;

int main(int argc, char* argv[]){
	int nw, g;

	get_conf(argc, argv, graph_file, needles, &nw, &g);

	if(nw <= 0 || g <= 0 ) {
        cout << "Error: n-workers and granularity must be grater than 0, ";
        cout  << wn << " and " << granularity << "  given.\n";
        exit(1);
    }

    #ifdef DEBUG
    cout << "Workers num: " << nw << ", Granularity: " << g << " \n";
    #endif 

    ManyLinesEmitter* em = new ManyLinesEmitter(g);

	vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
        Worker* w = new HasherWorker;
        
        #ifdef USE_AFFINITY
        w->setAffinity(j * 4);
        #endif
        
        workers.push_back(w);
    }

	ff_farm<string_task_t> graph_search_farm(workers, em );
	
	graph_search_farm->remove_collector();

	if(graph_search_farm.run_and_wait_end()<0) error("running farm");

	graph_file.close();
	return 0;
}

