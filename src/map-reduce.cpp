/** \file: map.cpp
*	\author Michele Carignani michele.carignani@gmail.com
*/
#define HAS_CXX11_VARIADIC_TEMPLATES 1

#include <ff/parallel_for.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>
#include <utility>

#include "graph-search.hpp"
#include "utils.hpp"

#define DEFAULT_REDUCTION 2000

using namespace std;
using namespace ff;

node_t* needles;

int main(int argc, char** argv){
    
    vector<char*> edgelist;
    ifstream graph_file;
    list<string> results;
    struct timespec start, end;
    #ifdef IO_TIME
    struct timespec end_io;
    #endif
    char* graph_filename;
    char buf[100];
    int nw, g, file_length, nsc, reduction;

    get_conf(argc, argv, &graph_filename, &needles, &nsc, &nw, &g);
    
    if(argc > 5){
        reduction = atoi(argv[5]);
        if(reduction <= 0){
            cerr << "Error: reduction must be a positive number, given" << reduction << endl;
            usage(argc, argv);
            cout << "<reduction>" << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        reduction = DEFAULT_REDUCTION;
    }
    
    if(g != 0 && reduction > g){
        reduction = g;
    }
    
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

    auto workerF = [&edgelist, &nsc, &reduction](const int start, const int end, 
            const int thid, ff_buffernode &node ){
        if (start == end) return;
        for(int k = start; k < end; k++){
            pair<node_t, node_t> res = parse_and_check_line(edgelist[k], needles, nsc);
            if(!res.first.is_null()){ 
                node.put(new pair<node_t,int>(res.first, k+1));
            }
            if(!res.second.is_null()){ 
                node.put(new pair<node_t,int>(res.second, k+1));
            }
        }
    };
    
    auto mergerF = [&results](void* t){
        pair<node_t, int>* p = (pair<node_t, int>*) t;
        list_found_node(&results, p->second,p->first);
        delete p;
    };
    
    // todo: controllare secondo parametro
    ParallelForPipeReduce <list<string>*> pfpipe (nw);
    if(is_set_granularity(argc) && g * nw < file_length-1 ) {
        // dynamic scheduling with stride g and par deg nw
        pfpipe.parallel_reduce_idx(0,file_length-1, 1, g, workerF, mergerF);
    } else {
        // static scheduling with auto strides
        // grain = 0 means static scheduling
        pfpipe.parallel_reduce_idx(0,file_length-1, 1, 0, workerF, mergerF);
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
