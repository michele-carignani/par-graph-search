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

    get_conf(argc, argv, &graph_file_path, &needles, &nw, &g);

    if(nw <= 0 || g <= 0 ) {
        cout << "Error: n-workers and granularity must be grater than 0, ";
        cout  << nw << " and " << g << "  given.\n";
        exit(1);
    }

    #ifdef DEBUG
    cout << "Workers num: " << nw << ", Granularity: " << g << " \n";
    #endif

#ifdef PRINT_EXEC_TIME
    float emitter_time = 0, collector_time = 0;
    float* workers_times = new float[nw];
    int emitter_execs = 0, collector_execs = 0;
    int* workers_execs = new int[nw];
#endif
    
    
#ifndef PRINT_EXEC_TIME
    ManyLinesEmitter em (graph_file_path, g);
    Collector col;
#else
    ManyLinesEmitter em (graph_file_path, g, &emitter_time, &emitter_execs);
    Collector col(&collector_time, &collector_execs);
#endif
    
    vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
#ifndef PRINT_EXEC_TIME
        ManyLinesWorker* w = new ManyLinesWorker (needles) ;
#else 
        workers_execs[j] = 0; workers_times[j] = 0;
        ManyLinesWorker* w = new ManyLinesWorker (needles, &(workers_times[j]), &(workers_execs[j])) ;
#endif
        
        #ifdef USE_AFFINITY
        w->setAffinity(j * 4);
        #endif
        
        workers.push_back(w);
    }

    ff_farm<> graph_search_farm (workers, &em, &col);
    
    clock_gettime(CLOCK_REALTIME, &start);
    if(graph_search_farm.run_and_wait_end()<0) error("running farm");    
    clock_gettime(CLOCK_REALTIME, &end);
   
#ifdef PRINT_EXEC_TIME
    cerr << elapsed_time_secs(start, end);
    cerr << "\nEmitter: " << emitter_execs << " times,\t" << emitter_time << " secs,\t"<< (emitter_time / emitter_execs) <<" avg\n";
    cerr << "Collector: " << collector_execs << " times,\t" << collector_time << " secs,\t"<< (collector_time / collector_execs) <<" avg\n";
    float avg_wt=0, avg_we=0;
    for(int j = 0; j < nw; j++){
        avg_wt += workers_times[j];
        avg_we += workers_execs[j];
    }
    cerr << "Workers avg: " << (avg_we / nw) << " times,\t" <<  (avg_wt / nw) << " secs,\t"<< (avg_wt / avg_we) <<" avg\n";;
    
#endif
    return 0;
}
