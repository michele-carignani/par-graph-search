/** \file: map.cpp
*	\author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/parallel_for.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>
#include <utility>

#include "graph-search.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

list<node_t> needles;
list<string> results;
list<string>* local_results;

int main(int argc, char** argv){
    
    vector<char*> edgelist;
    ifstream graph_file;
    struct timespec start, end;
    #ifdef IO_TIME
    struct timespec end_io;
    #endif
    char* graph_filename;
    char buf[100];
    int nw, g, file_length;

    get_conf(argc, argv, &graph_filename, &needles, &nw, &g);
    
    local_results = new list<string>[nw];
    
    graph_file.open(graph_filename);

    clock_gettime(CLOCK_REALTIME, &start);

    file_length = 0;
    graph_file.getline(buf, 100);
    while(graph_file.gcount() != 0){
        file_length++;
        edgelist.push_back(strdup(buf));
        graph_file.getline(buf, 100);        
    }
    
    #ifdef IO_TIME
    clock_gettime(CLOCK_REALTIME, &end_io);
    #endif

    auto workerF = [&edgelist](const int start, const int end, const int thid ){
        if (start == end) return;
        list<string>* my_local_results = &(local_results[thid]);
        for(int k = start; k < end; k++){
            pair<node_t, node_t> res = parse_and_check_line(edgelist[k], &needles);
            if(!res.first.is_null()){ list_found_node(my_local_results, k+1, res.first); }
            if(!res.second.is_null()){ list_found_node(my_local_results, k+1, res.second); }
        }
    };
    
    ParallelFor pf(nw);
    if(is_set_granularity(argc)){
        // dynamic scheduling with stride g and par deg nw
        pf.parallel_for_idx(0,file_length-1, 1, g, workerF);
    } else {
        // static scheduling with auto strides
        pf.parallel_for_idx(0,file_length-1, 1, 1, workerF);
    }
    
    for(int j = 0; j < nw; j++){
        results.merge(local_results[j]);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    
#ifdef PRINT_RESULTS
    for(auto x: results){
        cout << x << "\n";
    }
#endif
    cerr << elapsed_time_secs(start, end);
    #ifdef IO_TIME
    cerr << " : " << elapsed_time_secs(end_io, end);
    #endif
    return 0;
}
