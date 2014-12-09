/** file: graph-search.hpp
*
*
*/

#ifndef GRAPH_SEARCH_HPP
#define GRAPH_SEARCH_HPP

#define BUFSIZE 10000
#define TASK_SIZE 1000
#define LEFT_BUF_SIZE 100

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

char buf[BUFSIZE]; 

#endif