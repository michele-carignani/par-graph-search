/** \file seq.cpp
*   \author Michele Carignani <michele.carignani@gmail.com>
*/
#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include "graph-search.hpp"
#include "utils.hpp"

#define GRAPH_FILENAME_IDX 1
#define BUF_LEN 100

using namespace std;

node_t* needles;
list<string> results;

/**
		Usage: argv[0] graph_file needles_file
*/	

int main(int argc, char* argv[]){
    int nsc = 0;
    ifstream graph_file;
    char buf[BUF_LEN];
    struct timespec start, end;
    vector<char *> edgelist;

    load_needles_list(argc, argv, &needles, &nsc);

    graph_file.open(argv[GRAPH_FILENAME_IDX]);
    
    // Load the file in memory
    graph_file.getline(buf, 100);
    while(graph_file.gcount() != 0){ 
        edgelist.push_back(strdup(buf));
        graph_file.getline(buf, 100);
    }
    
    clock_gettime(CLOCK_REALTIME, &start);
    for(unsigned int j = 0; j < edgelist.size();  j++){
        
        pair<node_t,node_t> found = parse_and_check_line(edgelist[j], needles, nsc);
        
        if(!found.first.is_null()){
            list_found_node(&results , j+1, found.first);
        }
        if(!found.second.is_null()){
            list_found_node(&results, j+1, found.second);
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);

#ifdef PRINT_RESULTS
    for(auto x : results){
        cout << x << "\n";
    }
#endif
    
    cerr << elapsed_time_secs(start, end);

    return 0;
}