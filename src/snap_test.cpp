/* 
 * File:   snap_test.cpp
 * Author: miche
 *
 * Created on 25 febbraio 2015, 13.58
 */

#include <cstdlib>
#include <Snap.h>
#include "utils.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    int nsc;
    node_t* needles;
    list<node_t> found_nodes;
    struct timespec start, end;

    load_needles_list(argc, argv, &needles, &nsc);
    
    TNGraph Graph = TSnap::LoadEdgeList(argv[1], 0, 1);
    
    clock_gettime(CLOCK_REALTIME, &start);
    for (TNGraph::TNodeI NI = Graph.BegNI(); NI < Graph.EndNI(); NI++) {
        for(int i = 0 ; i < nsc; i++){
            if(node_t(NI.GetId()) == needles[i]){
                found_nodes.push_back(needles[i]);
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);

    cerr << elapsed_time_secs(start, end);
    
    return 0;
}

