/** \file: utils.hpp
*/

/** \file: utils.cpp
*
*
*/

#include "utils.hpp"

void check_usage(int argc, char** argv);

void open_graph_file(int argc, char** argv, ifstream graph_file);

void load_needles_list(int argc, char** argv, list<string> needles);

int get_workers_num(argc, argv);

int get_granularity(argc, argv);

void get_conf(int argc, char** argv, ifstream gf, list<string> ns, int* nw, int* g);
