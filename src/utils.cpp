/** \file: utils.cpp
*   \author: Michele Carignani <michele.carignani@gmail.com>
*
*/

#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include "utils.hpp"

using namespace std;

/* ********************* USAGE AND COMMAND LINE PARSING ********************* */

void usage(int argc, char** argv){
    cout << "Usage: " << argv[0];
    cout << " <graph-file> <needles-file> <n-workers> <granularity>\n";
    exit(0);
}

void open_graph_file(int argc, char** argv, char** input_path) {
    if(argc < 2){
        usage(argc, argv);
    }
    *input_path = argv[1];
}

void load_needles_list(int argc, char** argv, node_t** needles, int* needles_count){
    ifstream needles_file;
    if(argc < 3){
        usage(argc, argv);
    }

    needles_file.open(argv[2]);
    
    if(!needles_file){
        cerr << "Error opening needles file: " << argv[2] << ".\n";
        exit(0);
    }

    char buf[50];
    needles_file.getline(buf, 50);
    vector<node_t> nl;
    while(needles_file /*.gcount() != 0*/){
      nl.push_back(node_t(buf));
      needles_file.getline(buf, 50);   
    }
    
#ifdef ON_MIC
    *needles = __mm_malloc(nl.size() * sizeof(node_t),64);
#else  
    *needles = (node_t*) malloc(nl.size() * sizeof(node_t));
#endif
    std::copy(nl.begin(), nl.end(), *needles);
    *needles_count = nl.size();
}

int get_workers_num(int argc, char** argv){
    if(argc < 4){
        return DEFAULT_WORKERS_NUM;
    }
    return atoi(argv[3]);
}

int get_granularity(int argc, char** argv){
    if(argc < 5){
        return DEFAULT_GRANULARITY;
    }
    return atoi(argv[4]);
}

bool is_set_par_deg(int argc){
    return argc > 3;
}

bool is_set_granularity(int argc){
    return argc > 4;
}

void get_conf(int argc, char** argv, char** gf, node_t** ns, int* needles_count, int* nw, int* g){
    open_graph_file(argc, argv, gf);
    load_needles_list(argc, argv, ns, needles_count);
    *nw = get_workers_num(argc, argv);
    *g = get_granularity(argc, argv);
}

/* ********************** GRAPH SEARCH LOGIC ******************************* */

pair<node_t,node_t> parse_and_check_line(string* line, list<node_t>* needles){
    
    pair<node_t,node_t> result;
    
    if((*line)[0] == '#') return result;
    
    node_t first = node_t(line->substr(0, line->find("\t")));
    node_t second = node_t(line->substr(
            line->find("\t") + 1, 
            line->find("\r") - 1 - line->find("\t")
     ));
    
    list<node_t>::iterator it;
    
    for(it = needles->begin(); it != needles->end(); it++){
        if( *it == first ){
            result.first = *it;
        }
        if( *it == second ){
            result.second = *it;
        }
    }
    return result;
}

pair<node_t, node_t> parse_and_check_line(char* line, node_t* needles, int needles_count){
    pair<node_t, node_t> result;
    
    if(*line == '#') return result;
    
    node_t fir (strsep(&line, "\t"));
    node_t sec (line);
    
    for(int it = 0; it != needles_count; it++){
        if(fir ==  needles[it] ){
            result.first = needles[it];
        }
        if( sec == needles[it] ){
            result.second = needles[it];
        }
    }
    return result;
}

void list_found_node(list<string>* res, int linenum, node_t it){
    char* line_num = new char[50];
    
    char* str_val = it.str();
    sprintf(line_num, "%d : %s", linenum, str_val);
    free(str_val);

    res->push_back(string(line_num));
    delete[] line_num;
}

void print_found_node(single_task_t* t, string needle){
    cout << "Trovato " << needle << " alla riga "; 
    cout << ((single_task_t*) t)->linenum << "\n";
}

long embed_ints(int a, int b){
    long r = 0;
    return (( r | a) << 32) | b;
}

int split_first(long l){
    return (int) (l >> 32);
}

int split_second(long l){
    l = l << 32;
    return (int) (l >> 32);
}

/* *********************** PROFILING UTILIS ********************************* */

float elapsed_time_secs(struct timespec from, struct timespec to){
    float secs, nsecs;
    if((to.tv_nsec - from.tv_nsec) < 0){
        secs = to.tv_sec - from.tv_sec - 1;
        nsecs = 1000000000 + to.tv_nsec - from.tv_nsec;
    } else {
        secs = to.tv_sec - from.tv_sec;
        nsecs = to.tv_nsec - from.tv_nsec;
    }
    float r = secs + nsecs / ((1000.0) * (1000.0) * (1000.0));
    return r;
}

float elapsed_time_nsecs(struct timespec from, struct timespec to){
    long secs, nsecs;
    if((to.tv_nsec - from.tv_nsec) < 0){
        secs = to.tv_sec - from.tv_sec - 1;
        nsecs = 1000000000 + to.tv_nsec - from.tv_nsec;
    } else {
        secs = to.tv_sec - from.tv_sec;
        nsecs = to.tv_nsec - from.tv_nsec;
    }
    float r = (float) secs + (float) nsecs;
    return r;
}

/*
float profile_func(std::function<void()> func, clockid_t clock = CLOCK_REALTIME) {
    struct timespec start, end;
    clock_gettime(clock, &start);
    func();
    clock_gettime(clock, &end);
    return elapsed_time_secs(start,end);
}
 * */