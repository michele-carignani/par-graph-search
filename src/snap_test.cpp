/* 
 * File:   snap_test.cpp
 * Author: miche
 *
 * Created on 25 febbraio 2015, 13.58
 */

#include "../../../snap/snap-core/Snap.h"
#undef min
#undef max

#include <iostream>

#include "utils.hpp"

// using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    int nsc;
    node_t* needles;
    std::list<node_t> found_nodes;
    // struct timespec start, end;

    load_needles_list(argc, argv, &needles, &nsc);
    
    TNGraph Graph;
    Graph = TSnap::LoadEdgeList<TNGraph>(argv[1],0 , 1);
    
    // clock_gettime(CLOCK_REALTIME, &start);
    for (TNGraph::TNodeI EI = Graph.BegEI(); EI < Graph.EndEI(); EI++) {
        for(int i = 0 ; i < nsc; i++){
            if(node_t(EI.GetId()) == needles[i]){
                found_nodes.push_back(needles[i]);
            }
        }
    }
    // clock_gettime(CLOCK_REALTIME, &end);

    // cerr << elapsed_time_secs(start, end);
    
    return 0;
}

