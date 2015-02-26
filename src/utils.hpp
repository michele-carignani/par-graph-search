/** \file: utils.hpp
*	\author: Michele Carignani <michele.carignani@gmail.com>
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <list>
#include <string>
#include <functional>
#include <vector>
#include "graph-search.hpp"

void usage(int argc, char** argv);

void open_graph_file(int argc, char** argv, char** graph_file);

void load_needles_list(int argc, char** argv, node_t** needles, int* nsc);

int get_workers_num(int argc, char** argv);

int get_granularity(int argc, char** argv);

void get_conf(int argc, char** argv, char** gf, node_t** ns, int* nsc, int* nw, int* g);

bool is_set_par_deg(int argc);

bool is_set_granularity(int argc);

/** \return a pair containing zero, one or two strings found in the line
 *
 */
std::pair<node_t, node_t> parse_and_check_line(std::string* line, std::list<node_t>* needles);
std::pair<node_t, node_t> parse_and_check_line(char* line,node_t* needles, int needles_count);

void list_found_node(std::list<std::string>* res, int linenum, node_t needle);

void print_found_node(single_task_t* t, std::string needle);

long embed_ints(int a, int b);
int split_first(long l);
int split_second(long l);

float elapsed_time_secs(struct timespec from, struct timespec to);
float elapsed_time_nsecs(struct timespec from, struct timespec to);

// float profile_func(lambda() func, clockid_t clock);


#endif // UTILS_HPP
