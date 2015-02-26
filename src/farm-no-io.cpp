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
node_t* needles;

int main(int argc, char** argv) {
    int nw, g, i, needles_count;
    char* graph_file_path;
    struct timespec start, end;
    #ifdef IO_TIME
    struct timespec end_io;
    #endif
    vector<char*> edgelist;
    ifstream graph_file;
    char buf[100];

    get_conf(argc, argv, &graph_file_path, &needles, &needles_count, &nw, &g);

    if(nw <= 0 || g <= 0 ) {
        cout << "Error: n-workers and granularity must be grater than 0, ";
        cout  << nw << " and " << g << "  given.\n";
        exit(1);
    }
    
    #ifdef DEBUG
    cout << "Workers num: " << nw << ", Granularity: " << g << " \n";
    #endif
    
    // Load edge list into memory
    
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

#ifdef PRINT_EXEC_TIME
    float emitter_time = 0, collector_time = 0;
    float* workers_times = new float[nw];
    int emitter_execs = 0, collector_execs = 0;
    int* workers_execs = new int[nw];
#endif
    
#ifndef PRINT_EXEC_TIME
    IteratorEmitter em (&edgelist, g);
    Collector col;
#else
    IteratorEmitter em (&edgelist, g, &emitter_time, &emitter_execs);
    Collector col(&collector_time, &collector_execs);
#endif
    vector<ff_node *> workers;
    for(int j = 0; j < nw; j++){
#ifndef PRINT_EXEC_TIME
        IteratorWorker* w = new IteratorWorker (&edgelist, needles, needles_count) ;
#else 
        workers_execs[j] = 0; workers_times[j] = 0;
        IteratorWorker* w = new IteratorWorker (needles, needles_count, &(workers_times[j]), &(workers_execs[j])) ;
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
    
    #ifdef IO_TIME
    cerr << " : " << elapsed_time_secs(end_io, end);
    #else 
    cerr << elapsed_time_secs(start, end);
    #endif

#ifdef PRINT_EXEC_TIME

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

