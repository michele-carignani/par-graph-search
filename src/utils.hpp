/** \file: utils.hpp
*	\author: Michele Carignani <michele.carignani@gmail.com>
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <list>
#include <string>
#include "graph-search.hpp"

void usage(int argc, char** argv);

void open_graph_file(int argc, char** argv, char** graph_file);

void load_needles_list(int argc, char** argv, std::list<std::string>* needles);

int get_workers_num(int argc, char** argv);

int get_granularity(int argc, char** argv);

void get_conf(int argc, char** argv, char** gf, std::list<std::string>* ns, int* nw, int* g);

bool is_set_par_deg(int argc);

bool is_set_granularity(int argc);

void parse_and_check_line(single_task_t task, std::list<std::string> needles, std::list<std::string>* res);

void list_found_node(std::list<std::string>* res, int linenum, std::string needle);

void print_found_node(single_task_t* t, std::string needle);

float elapsed_time_secs(struct timespec from, struct timespec to);

#endif // UTILS_HPP
