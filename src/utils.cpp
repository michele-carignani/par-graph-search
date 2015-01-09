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
        cout << "errore apertura file\n";
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
    return argc > 2;
}

bool is_set_granularity(int argc){
    return argc > 3;
}

void get_conf(int argc, char** argv, char** gf, list<string>* ns, int* nw, int* g){
    open_graph_file(argc, argv, gf);
    load_needles_list(argc, argv, ns);
    *nw = get_workers_num(argc, argv);
    *g = get_granularity(argc, argv);
}

void parse_and_check_line(single_task_t task, list<string> needles){

    if(task.line[0] == '#') return;
    
    string first = task.line.substr(0, task.line.find("\t"));
    string second = task.line.substr(task.line.find("\t") + 1, task.line.find("\r") - 1 - task.line.find("\t"));
    
    list<string>::iterator it;

    for(it = needles.begin(); it != needles.end(); it++){
           // cout << "Comparo " << (*it) << " e " << first << " e " << second << "\n";
           if((*it).compare(first) == 0){
               print_found_node(&task, *it);
           }
           if((*it).compare(second) == 0){
               print_found_node(&task, *it);
           }
       }
}

void print_found_node(single_task_t* t, string needle){
    cout << "Trovato " << needle << " alla riga " << ((single_task_t*) t)->linenum << "\n";
}

float elapsed_time_secs(struct timespec from, struct timespec to){
    float r = ((float)to.tv_nsec) - ((float)from.tv_nsec);
    return r / ((1000.0) * (1000.0) * (1000.0));
}
