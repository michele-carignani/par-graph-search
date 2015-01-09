/** file: graph-search.hpp
*
*
*/

#include <string>
#include <list>
#include <time.h>

#ifndef _GRAPH_SEARCH_HPP
#define _GRAPH_SEARCH_HPP

#define BUFSIZE 10000
#define TASK_SIZE 1000
#define LEFT_BUF_SIZE 100
#define DEFAULT_GRANULARITY 20
#define DEFAULT_WORKERS_NUM 2

typedef struct string_task {
	// First character of chunk
	char* start;

	// Number of characters before last \n
	int len;
public:
	string_task(char* s, int l): start(s), len(l) {}
} string_task_t;

typedef struct bytes_task {
	int start = 0;
	int size = 0;

public:
	bytes_task(int st, int si): start(st), size(si) {}
} bytes_task_t;

typedef struct single_line_task {
    public:
    single_line_task(): line(""), linenum(-1) {};
    single_line_task(std::string l, int ln): line(l), linenum(ln) {};
    std::string line;
    int linenum;
} single_task_t;

typedef struct multi_lines_task {
    private:
    int size = 1;
    int count = 0;

    public:
    multi_lines_task(int size){
        lines = new single_task_t[size];
    };

    void add_task(std::string line, int num){
        lines[count].linenum = num;
        lines[count].line = std::string(line);
        count++;
    }

    int get_count(){
        return count;
    }

    single_task_t* lines;
} multi_task_t;

#endif
