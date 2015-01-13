/** \file: map.cpp
*	\author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/parallel_for.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>

#include "graph-search.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

list<string> needles;

int main(int argc, char** argv){

    vector<single_task_t> edgelist;
    ifstream graph_file;
    struct timespec start, all_read, end;

    char* graph_filename;
    char buf[100];
    int nw, g, i;

    get_conf(argc, argv, &graph_filename, &needles, &nw, &g);

    graph_file.open(graph_filename);

    clock_gettime(CLOCK_REALTIME, &start);

    i = 1;
    graph_file.getline(buf, 100);
    while(graph_file.gcount() != 0){
        edgelist.push_back(single_task_t(buf, i));
        i++;
        graph_file.getline(buf, 100);        
    }

    clock_gettime(CLOCK_REALTIME, &all_read);

    ParallelFor pf;
    if(is_set_par_deg(argc)){
        if(is_set_granularity(argc)){
            // dynamic scheduling with stride g and par deg nw
            pf.parallel_for(0,i-1, nw, g, [&edgelist](const int i){
                parse_and_check_line(edgelist[i], needles);
            });
        } else {
            // static scheduling with auto strides
            pf.parallel_for(0,i-1, nw, [&edgelist](const int i){
                parse_and_check_line(edgelist[i], needles);
            });
        }
    } else {
        // dynamic scheduling with auto par deg and auto strides
        pf.parallel_for(0,i-1, [&edgelist](const int i){
            parse_and_check_line(edgelist[i], needles);
        });
    }
    
    clock_gettime(CLOCK_REALTIME, &end);
	   
    cerr << elapsed_time_secs(start, end);

    return 0;
}
