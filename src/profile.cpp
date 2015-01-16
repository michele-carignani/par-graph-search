/* 
 * File:   profile.cpp
 * Author: miche
 *
 * Created on 16 gennaio 2015, 15.57
 */
#include <time.h>
#include "utils.hpp"

using namespace std;

#define ITER 50000

/*
 * 
 */
int main(int argc, char** argv) {

    char* line = "50983\t820394\r\n";
    char* needle = "50983";
    
    struct timespec start_r, end_r, start_cpu, end_cpu;
    
    single_task_t task(line, 1);
    list<string> needles;
    needles.push_back(needle);
    
    list<string> results;
    
    clock_gettime(CLOCK_REALTIME, &start_r);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);
    
    for(int i = 0; i < ITER; i++){
        parse_and_check_line(task, needles, *results);
    }
    
    clock_gettime(CLOCK_REALTIME, &end_r);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);
    
    cout << "Tc of f (REAL) : " << elapsed_time_secs(start_r,end_r) / ITER << "\n";
    cout << "Tc of f (CPU) : " << elapsed_time_secs(start_cpu,end_cpu) / ITER << "\n";
    
    return 0;
}

