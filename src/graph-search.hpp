/** \file: graph-search.hpp
*   \author: Michele Carignani <michele.carignani@gmail.com>
*
*/
#ifndef _GRAPH_SEARCH_HPP
#define _GRAPH_SEARCH_HPP

#include <cstring>
#include <cstdlib>
#include <string>
#include <list>
#include <vector>
#include <time.h>

// todo: dynamic default_granularity
#define DEFAULT_GRANULARITY 300
#define DEFAULT_WORKERS_NUM 2

// todo controllare lunghezza parola
#define MAX_NODE_LENGTH 50

/* ************************* INT_NODE_T ************************************* */

typedef struct int_node_struct {
	public:
#ifdef ON_MIC
	unsigned int nval __attribute__((aligned(64)));
#else
	unsigned int nval;
#endif
	
	int_node_struct(unsigned int n) : nval(n) {};
	int_node_struct(char* s)  { nval = atoi(s); }; 
	int_node_struct(std::string s)  { nval = atoi(s.c_str()); }; 
	int_node_struct() { nval = 0; }
	
	bool operator==(int_node_struct n1){
		return n1.nval == this->nval;
	}
	
	bool is_null(){
		return nval == 0;
	}
	
	char* str(){
		char* res = (char*) malloc(sizeof(char) * MAX_NODE_LENGTH);
		sprintf(res, "%d", nval);
		return res;
	}
	
} int_node_t;

#define INT_NULL_NODE node_t(0);

/* **************************** STR_NODE_T ********************************** */

typedef struct str_node_struct {
	public:
	
#ifdef ON_MIC
	char sval[MAX_NODE_LENGTH] __attribute__((aligned(64)));
#else
	char sval[MAX_NODE_LENGTH];
#endif
	
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
	
	str_node_struct(int n){
		snprintf(sval,MAX_NODE_LENGTH, "%ul", n );
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
	
	char* str(){
		return strdup(sval);
	}
	
} str_node_t;
#define STR_NULL_NODE str_node_t({'\0'})

#ifdef USE_INT_NODES
#define NULL_NODE node_t(0)
typedef int_node_t node_t;
#else
#define NULL_NODE node_t({'\0'})
typedef str_node_t node_t;
#endif

// todo
// alignement

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

// todo:
// merge the two structures
// alignement

/**
*   A task composed of many edges (represented as
*   single_task_t structs), used to increase computation grain in
*   the workers;
*/
typedef struct multi_lines_task {
    private:
    int size;
    int count;
    
    public:
    single_task_t* lines; /** The edges assigned to the task */
    
    multi_lines_task(int s){
		size = s;
		count = 0;
        lines = new single_task_t[s];
    };    
    
    ~multi_lines_task(){
        delete[] lines;
    }

    void add_task(char* line, int num){
        lines[count].linenum = num;
		// todo passare a memoria statica, allineare
        lines[count].line = strdup(line);
        count++;
    }

    int get_count(){
        return count;
    }
    
} multi_task_t;


#endif
