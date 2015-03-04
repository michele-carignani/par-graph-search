/** \file nodes.hpp
*   \author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include "graph-search.hpp"
#include "utils.hpp"

#ifndef _NODES_HPP
#define _NODES_HPP

/**
 *  A class defining a node, to be used as
 *  an emitter of tasks of type multi_task_t.
 *  The input data is read from a text file.
 * 
 */
class ManyLinesEmitter : public ff::ff_node {
    private:
    int linenum; /** Keeps track of number of lines read */
    int granularity; /** Size of the tasks emitted */
    std::ifstream graph_file; /** Data file containing graph as an edge list */

	#ifdef PRINT_EXEC_TIME
    float* executed_secs; /** Keeps track of execution cpu time  fo the node */
	int* svc_executions;
	#endif

   public:
   void * svc(void  * t);
   
   /**
    * 
    * @param pathname Pathname of the input file
    * @param g Size of the task to be emitted. Default is 20
    */
   ManyLinesEmitter(char* pathname, int g);
   #ifdef PRINT_EXEC_TIME
	ManyLinesEmitter(char* pathname, int g, float* ex_secs, int* execs);
	#endif
   ~ManyLinesEmitter() {};

};

/* **************************** MANY LINES WORKER *************************** */

/** 
 *  Defines a worker that takes a multi_task_t task and 
 *  loop over the lines contained. Every time a node is 
 *  found, the found_node() method is called.
 */
class ManyLinesWorker : public ff::ff_node {
    protected:
    node_t* needles; /** List of nodes to be looked for */
	int needles_count;
#ifdef PRINT_EXEC_TIME
    float* executed_secs;
	int* svc_executions;
#endif
    
    public:
    ManyLinesWorker (node_t* ns, int nsc) {
		needles_count = nsc;
		needles = (node_t*) malloc(sizeof(node_t) * nsc);
		memcpy(needles, ns, sizeof(node_t) * nsc);
	};
	#ifdef PRINT_EXEC_TIME
	ManyLinesWorker(node_t* ns, int nsc, float* ex_secs, int* execs) : 
		executed_secs(ex_secs), svc_executions(execs) {
		needles_count = nsc;
		needles = (node_t*) malloc(sizeof(node_t) * nsc);
		memcpy(needles, ns, sizeof(node_t) * nsc);
	};
	#endif
    ~ManyLinesWorker () {};
    void * svc(void* t);
    void svc_end();

};

/** A class for a worker that simply prints to stdout
 *  when a node is founde: for each node found, a line is
 *  printed with node ID and line number.
 */
class PrinterWorker : public ManyLinesWorker {
    public:
    PrinterWorker(node_t* ns, int nsc) : ManyLinesWorker(ns, nsc) {};
    void found_node(int linenum, node_t needle);
};

 /** Describes a simple collector for graph search.
 *   It receives a string with node ID and line number
 *   and stores it in a list.
 */
class Collector : public ff::ff_node {
	#ifdef PRINT_EXEC_TIME
    float* executed_secs;
	int* svc_executions;
	#endif
    public:
    std::list<std::string> found_nodes;
	#ifdef PRINT_EXEC_TIME
	Collector(float* ex_secs, int* execs) : 
		executed_secs(ex_secs), svc_executions(execs) {};
	#endif
	

    void * svc(void * t);
    void svc_end();
    void print_res();
};

/* ************************** ITERATOR EMITTER *****************************  */
class IteratorEmitter : public ff::ff_node {
private:
	int curr;
	std::vector<char*>* graph;
	int granularity;
	#ifdef PRINT_EXEC_TIME
	float* executed_secs;
	int* svc_executions;
	#endif
	
public:
	IteratorEmitter (std::vector<char*>* graph, int g = 20) : 
		curr(0), graph(graph), granularity(g) {};
	
	#ifdef PRINT_EXEC_TIME
	IteratorEmitter(std::vector<char*>* graph, int g, float* ex_secs, int* execs) :
		curr(0),  graph(graph), granularity(g), executed_secs(ex_secs), svc_executions(execs) {};
	#endif
		
	void* svc(void* t);
};

/* ************************** ITERATOR WORKER *****************************  */
class IteratorWorker : public ManyLinesWorker {
	public:
	std::vector<char*>* graph;
	IteratorWorker (std::vector<char*>* gr, node_t* ns, int nsc):
		ManyLinesWorker(ns, nsc), graph(gr) {};
	#ifdef PRINT_EXEC_TIME
	IteratorWorker (std::vector<char*>* gr, node_t* ns, int nsc, float* ex_secs, int* execs) :
		ManyLinesWorker(ns, nsc, ex_secs, execs), graph(gr) {};
	#endif
	void* svc(void* t);
	
};

/* ************************** PROFILED NODE ******************************** */
class ProfiledNode : public ff::ff_node {
	float* executed_secs;
	int* svc_executions;
	
	void* svc(void* t);
};

#endif // _NODES_HPP