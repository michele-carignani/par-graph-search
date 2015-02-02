/** \file: seq.cpp
*	\author Michele Carignani <michele.carignani@gmail.com>
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

list<string> needles;
list<string> results;

/**
		Usage: argv[0] graph_file needles_file
*/	

int main(int argc, char* argv[]){

    ifstream graph_file;
    int linenum;
    char buf[BUF_LEN];
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);

    load_needles_list(argc, argv, &needles);

    graph_file.open(argv[GRAPH_FILENAME_IDX]);
    graph_file.getline(buf, BUF_LEN);
    linenum = 1;
    while(graph_file.gcount() != 0){
        string line (buf);
        pair<node_t,node_t> found = parse_and_check_line(&line, &needles);
        
        if(found.first != NULL_NODE){
            list_found_node(&results , linenum, found.first);
        }
        if(found.second != NULL_NODE){
            list_found_node(&results, linenum, found.first);
        }
        linenum++;
        graph_file.getline(buf, BUF_LEN);
    }

    clock_gettime(CLOCK_REALTIME, &end);

    cerr << elapsed_time_secs(start, end);

    return 0;
}