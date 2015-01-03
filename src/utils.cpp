/** \file: utils.cpp
*
*
*/

#include "utils.hpp"

void check_usage(int argc, char** argv){
    if(argc < 4){
        cout << "Usage: " << argv[0] << " <graph-file> <needles-file> <n-workers> <granularity>\n";
        exit(0);
    }
}

void open_graph_file(int argc, char** argv, ifstream graph_file) {

    if(argc < 2){
        usage();
    }

    char* filein = argv[1];
    graph_file.open(filein);

    if(!graph_file){
        perror("Opening graph file");
        exit(0);
    }
}

void load_needles_list(int argc, char** argv, list<string> needles){
    ifstream needles_file;

    if(argc < 3){
        usage(argc, argv);
    }

    needles_file.open(argv[2]);
    char buf[50];
    needles_file.getline(buf, 50);

    while(needles_file.gcount() != 0){
      needles.push_back(string(buf));
      needles_file.getline(buf, 50);   
    }
}

int get_workers_num(argc, argv){
    if(argc < 4){
        return DEFAULT_WORKERS_NUM;
    }
    return atoi(argv[3]);
}

int get_granularity(){
    if(argc < 5){
        return DEFAULT_GRANULARITY;
    }
    return atoi(argv[4]);
}

void get_conf(int argc, char** argv, ifstream gf, list<string> ns, int* nw, int* g){
    open_graph_file(argc, argv, gf);
    load_needles_list(argc, argv, ns);
    *nw = get_workers_numeber(argc, argv);
    *g = get_granularity(argc, argv);
}
