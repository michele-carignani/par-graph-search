/* 
 * File:   pure-map.cpp
 * Author: Michele Carignani
 *
 * Created on 20 gennaio 2015, 10.47
 */

#include <cstdlib>
#include <random>
#include <cmath>
#include <iostream>
#include "ff/parallel_for.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

#define DATA_SIZE 100000

/*
 * 
 */
int main(int argc, char** argv) {
    struct timespec start, end;
    
    double* data = new double[DATA_SIZE];
    
    for(int j = 0; j < DATA_SIZE; j++){
        data[j] = rand() % 1000;
    }
    
    cout << "# nworkers, process cpu time\n";
    for(int j = 1; j < 129; j++){
        ParallelFor pf(j);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        pf.parallel_for(0, DATA_SIZE, [&data](const int i){
            data[i] = pow(data[i], 7);
        });    
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        cout << j << ", " << elapsed_time_secs(start, end) << "\n";
    }
    
    return 0;
}

