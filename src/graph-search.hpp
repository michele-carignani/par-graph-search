/** \file: graph-search.hpp
*   \author: Michele Carignani <michele.carignani@gmail.com>
*
*/
#ifndef _GRAPH_SEARCH_HPP
#define _GRAPH_SEARCH_HPP

#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <time.h>

#define DEFAULT_GRANULARITY 20
#define DEFAULT_WORKERS_NUM 2

// todo controllare lunghezza parola
#define MAX_NODE_LENGTH 50

/* ************************* INT_NODE_T ************************************* */

typedef struct int_node_struct {
	public:
		// todo allineare
	unsigned int nval;
	
	int_node_struct(unsigned int n) : nval(n) {};
	int_node_struct(char* s)  { nval = atoi(s); }; 
	
	bool operator==(int_node_struct n1){
		return n1.nval == this->nval;
	}
	
} int_node_t;

#define INT_NULL_NODE node_t(0);

/* **************************** STR_NODE_T ********************************** */

typedef struct str_node_struct {
	public:
	
	// todo allineare
	char sval[MAX_NODE_LENGTH];
	
	str_node_struct(){
		sval[0] = '\0';
	}
	
	str_node_struct(char* s){
		strncpy(sval, s, MAX_NODE_LENGTH);
		char* carrier  = index(sval, '\r');
		if(carrier != NULL){
			*carrier = '\0';
		}
	}
	
	str_node_struct(std::string s){
		strncpy(sval, s.c_str(), MAX_NODE_LENGTH);
	}
	
	bool operator==(str_node_struct n1){
		int i = 0;
		do {
			if(n1.sval[i] != this->sval[i]) return false;
			i++;
		} while(this->sval[i] != '\0');
		return true;
	}
	
	void operator=(str_node_struct n1){
		strncpy(this->sval, n1.sval, MAX_NODE_LENGTH);
	}
	
	bool is_null(){
		return sval[0] == '\0';
	}
	
} str_node_t;

#define STR_NULL_NODE str_node_t({'\0'})

typedef str_node_t node_t;

#define NULL_NODE node_t({'\0'})
/** 
*    A single task for parallel graph search.
*    Represents an edge i.e. a line of the graph
*    file in the SNAP library format.
*/
typedef struct single_line_task {
    public:
    single_line_task(): line(NULL), linenum(-1) {};
    single_line_task(char* l, int ln) {
		linenum = ln;
		line = strdup(l);
	};
    ~single_line_task(){};
    char* line; /** The edge represented as a string */
    int linenum; /** The position in the graph file */
} single_task_t;


/**
*   A task composed of many edges (represented as
*   single_task_t structs), used to increase computation grain in
*   the workers;
*/
typedef struct multi_lines_task {
    private:
    int size = 1;
    int count = 0;
    
    public:
    single_task_t* lines; /** The edges assigned to the task */
    
    multi_lines_task(int size){
        lines = new single_task_t[size];
    };    
    
    ~multi_lines_task(){
        delete[] lines;
    }

    void add_task(char* line, int num){
        lines[count].linenum = num;
        lines[count].line = line;
        count++;
    }

    int get_count(){
        return count;
    }
    
} multi_task_t;

/** 
*   \deprecated
*   Represents a portion of the edgelist
*   file, in particular contains len lines.
*/
typedef struct string_task {
    // First character of chunk
    char* start;

    // Number of characters before last \n
    int len;
public:
    string_task(char* s, int l): start(s), len(l) {};
} string_task_t;


/**
*   \deprecated
*   Describes a portion of the edgelist
*   as an array of bytes. It is then parsed to find
*   newlines characters.
*/
typedef struct bytes_task {
    int start = 0;
    int size = 0;

public:
    bytes_task(int st, int si): start(st), size(si) {};
} bytes_task_t;

typedef struct iterator_task {
	std::vector<char*>* graph;
	unsigned int start;
	unsigned int end;
	iterator_task(std::vector<char*>* g, int s,int e) :
		graph(g), start(s), end(e) {};
} it_task_t ;

#endif
