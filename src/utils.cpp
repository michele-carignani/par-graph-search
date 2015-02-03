/** \file: utils.cpp
*   \author: Michele Carignani <michele.carignani@gmail.com>
*
*/

#include <iostream>
#include <list>
#include <fstream>
#include "utils.hpp"

using namespace std;

void usage(int argc, char** argv){
    cout << "Usage: " << argv[0] << " <graph-file> <needles-file> <n-workers> <granularity>\n";
    exit(0);
}

void open_graph_file(int argc, char** argv, char** input_path) {
    if(argc < 2){
        usage(argc, argv);
    }
    *input_path = argv[1];
}

void load_needles_list(int argc, char** argv, list<string>* needles){
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
    
    while(needles_file /*.gcount() != 0*/){
      needles->push_back(string(buf));
      needles_file.getline(buf, 50);   
    }

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

void get_conf(int argc, char** argv, char** gf, list<string>* ns, int* nw, int* g){
    open_graph_file(argc, argv, gf);
    load_needles_list(argc, argv, ns);
    *nw = get_workers_num(argc, argv);
    *g = get_granularity(argc, argv);
}

pair<node_t,node_t> parse_and_check_line(string* line, list<node_t>* needles){

    pair<node_t,node_t> result;
    result.first = NULL_NODE;
    result.second = NULL_NODE;
    
    if((*line)[0] == '#') return result;
    
    string first = line->substr(0, line->find("\t"));
    string second = line->substr(
            line->find("\t") + 1, 
            line->find("\r") - 1 - line->find("\t")
     );
    
    list<string>::iterator it;
    
    for(it = needles->begin(); it != needles->end(); it++){
        if((*it).compare(first) == 0){
            result.first = (node_t) *it;
        }
        if((*it).compare(second) == 0){
            result.second = (node_t) *it;
        }
    }
    return result;
}

void list_found_node(list<string>* res, int linenum, string it){
     char* line_num = new char[50];
    
    sprintf(line_num, "%d :",linenum);
    string f (line_num);
    f.append(it);

    res->push_back(f);
    delete[] line_num;
}

void print_found_node(single_task_t* t, string needle){
    cout << "Trovato " << needle << " alla riga " << ((single_task_t*) t)->linenum << "\n";
}

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