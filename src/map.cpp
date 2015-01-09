/** \file: map.cpp
	\author Michele Carignani michele.carignani@gmail.com
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
    int nw, g, i, k, r;

    get_conf(argc, argv, &graph_filename, &needles, &nw, &g);

    graph_file.open(graph_filename);
    cout << "Leggo il file " << graph_filename << "\n";

    clock_gettime(CLOCK_REALTIME, &start);

    i = 1;
    graph_file.getline(buf, 100);
    while(graph_file.gcount() != 0){
        // cout << buf << std::endl;
        edgelist.push_back(single_task_t(buf, i));
        i++;
        graph_file.getline(buf, 100);
        // k = graph_file.gcount();
        
    }

    clock_gettime(CLOCK_REALTIME, &all_read);

    ParallelFor pf;
    pf.parallel_for(0,i-1, [&edgelist](const int i){
        // cout << i;   //<< "\n";
       parse_and_check_line(edgelist[i], needles);
    });
    
    clock_gettime(CLOCK_REALTIME, &end);
	   
    cerr << elapsed_time_secs(start, end);
    // cerr << "Total: " << elapsed_time_secs(start, end) << std::endl;
    //cerr << "Reading: " << elapsed_time_secs(start, all_read) << std::endl;
    //cerr << "Parsing: " << elapsed_time_secs(all_read, end) << std::endl;

	return 0;
}
