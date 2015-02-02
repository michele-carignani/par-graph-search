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

list<string> needles;
list<string> results;

int main(int argc, char** argv){
    
    vector<single_task_t> edgelist;
    ifstream graph_file;
    struct timespec start, end;
    #ifdef IO_TIME
    struct timespec end_io;
    #endif
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
    
    #ifdef IO_TIME
    clock_gettime(CLOCK_REALTIME, &end_io);
    #endif

    if(is_set_par_deg(argc)){
        ParallelFor pf(nw);
        if(is_set_granularity(argc)){
            // dynamic scheduling with stride g and par deg nw
            pf.parallel_for(0,i-1, 1, g, [&edgelist](const int i){
                string l (edgelist[i].line);
                pair<node_t, node_t> res = parse_and_check_line(&l, &needles);
                if(res.first != NULL_NODE){
                    list_found_node(&results, i, res.first);
                }
                if(res.second != NULL_NODE){
                    list_found_node(&results, i, res.second);
                }
                
            });
        } else {
            // static scheduling with auto strides
            pf.parallel_for(0,i-1, 1, [&edgelist](const int i){
                string l (edgelist[i].line);
                pair<node_t, node_t> res = parse_and_check_line(&l, &needles);
                if(res.first != NULL_NODE){
                    list_found_node(&results, i, res.first);
                }
                if(res.second != NULL_NODE){
                    list_found_node(&results, i, res.second);
                }
            });
        }
    } else {
        ParallelFor pf;
        // dynamic scheduling with auto par deg and auto strides
        pf.parallel_for(0,i-1,  [&edgelist](const int i){
            string l (edgelist[i].line);
            pair<node_t, node_t> res = parse_and_check_line(&l, &needles);
            if(res.first != NULL_NODE){
                list_found_node(&results, i, res.first);
            }
            if(res.second != NULL_NODE){
                list_found_node(&results, i, res.second);
            }
        });
    }
    
    clock_gettime(CLOCK_REALTIME, &end);
    for(auto x: results){
        cout << x << "\n";
    }
    cerr << elapsed_time_secs(start, end);
    #ifdef IO_TIME
    cerr << " : " << elapsed_time_secs(end_io, end);
    #endif
    return 0;
}
