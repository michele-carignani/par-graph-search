/* 
 * File:   profile.cpp
 * Author: miche
 *
 * Created on 16 gennaio 2015, 15.57
 */
#include <time.h>
#include <iostream>
#include "utils.hpp"
#include <random>

using namespace std;

#define ITER 50000
#define MAX_NEEDLES 100

/*
 * 
 */
int main(int argc, char** argv) {

    string line = "50983\t820394\r\n";
    string needle = "50983";
    
    struct timespec start_r, start_cpu, end_r, end_cpu;
    float realtimes[MAX_NEEDLES], cputimes[MAX_NEEDLES]; 
    single_task_t task(line, 1);
    list<string> needles;
    needles.push_back(needle);
    int new_needle;
    list<string> results;
    
    for(int j = 0; j < MAX_NEEDLES; j++){
        needles.push_back(std::to_string(rand()));
        
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);
        clock_gettime(CLOCK_REALTIME, &start_r);
        
        for(int i = 0; i < ITER; i++){
            parse_and_check_line(task, needles, &results);
        }
        
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);
        clock_gettime(CLOCK_REALTIME, &end_r);
        
        realtimes[j] = elapsed_time_nsecs(start_r, end_r);
        cputimes[j] = elapsed_time_nsecs(start_cpu, end_cpu);

    }
    
    cout << "## Tempo medio di esecuzione di parse_and_check_line() per 1 needle\n";
    cout << "Needles; Realtime; Cputime\n";
    
    for(int j = 0; j < MAX_NEEDLES; j++){
        cout <<  (j+1) << "; " << realtimes[j] / (ITER * (j+1));
        cout << "; " << cputimes[j] / (ITER * (j+1)) << "\n";
    }
    
    return 0;
}

