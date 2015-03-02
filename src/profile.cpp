/* 
 * File:   profile.cpp
 * Author: miche
 *
 * Created on 16 gennaio 2015, 15.57
 */
#include <time.h>
#include <iostream>
#include "utils.hpp"
#include "graph-search.hpp"
#include <random>

using namespace std;

#define ITER 50000

/*
 * 
 */
int main(int argc, char** argv) {

    cout << " --- " << endl;
    printf("Sizeof(node_t): %lu bytes\n", sizeof(node_t));
        
    struct timespec start_r, start_cpu, end_r, end_cpu;
    float realtime, cputime; 
    node_t* needles;
    list<string> results;
    int nsc;
    string task = "50123\t12314\r\n";
    char* ctask = (char*) task.c_str();
    
    load_needles_list(argc, argv, &needles, &nsc);
    // list<string> results;
        
    cout << "# Needles: "<< nsc << endl;
    cout << "Sizeof(needles): " << (sizeof(node_t) * nsc  ) << " bytes" << endl;
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);
    clock_gettime(CLOCK_REALTIME, &start_r);
    
    for(int i = 0; i < ITER; i++){
        // char* ctaskp = strdup(ctask);
        pair<node_t,node_t> found = parse_and_check_line(ctask, needles, nsc);
        
        if(!found.first.is_null()){
            list_found_node(&results , i+1, found.first);
        }
        if(!found.second.is_null()){
            list_found_node(&results, i+1, found.second);
        }
        
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);
    clock_gettime(CLOCK_REALTIME, &end_r);

    realtime= elapsed_time_nsecs(start_r, end_r);
    cputime = elapsed_time_nsecs(start_cpu, end_cpu);
    
    cout << "## Tempo medio di esecuzione di parse_and_check_line()";
    cout << " sulla lista di needles data (in media su " << ITER << " esecuzioni)" << endl;
    
    cout << "Realtime " << realtime / ITER << " nsecs" << endl;
    cout << "Cputime " << cputime / ITER<< " nsecs" << endl;
    cout << "-- "<< endl << endl;
    return 0;
}

