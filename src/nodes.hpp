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

/**
 *  A class describing an emitter node that takes
 *  the input data from a vector of strings (without performing
 *  IO operations).
 */
class EmitterNoIO : public ff::ff_node {
    private:
        unsigned int linenum;
        
	protected:
		std::vector<char*>* graph;
		int granularity;
		#ifdef PRINT_EXEC_TIME
        float* executed_secs;
		int* svc_executions;
		#endif
		
    public:
        /**
         * 
         * @param graph Vector of lines of the edgelist file
         * @param g Size of the multi_task_t task
         */
        EmitterNoIO(std::vector<char*>* graph, int g = 20):
            linenum(0), graph(graph), granularity(g){};
		
		#ifdef PRINT_EXEC_TIME
		EmitterNoIO(std::vector<char*>* graph, int g, float* ex_secs, int* execs) : 
			linenum(0), graph(graph), granularity(g),
			executed_secs(ex_secs), svc_executions(execs) {};
		#endif
            
        ~EmitterNoIO() {};
        void* svc(void * t);
        void svc_end();
};

/* **************************** MANY LINES WORKER *************************** */

/** 
 *  Defines a worker that takes a multi_task_t task and 
 *  loop over the lines contained. Every time a node is 
 *  found, the found_node() method is called.
 */
class ManyLinesWorker : public ff::ff_node {
    protected:
    std::list<node_t> needles; /** List of nodes to be looked for */
#ifdef PRINT_EXEC_TIME
    float* executed_secs;
	int* svc_executions;
#endif
    
    public:
    ManyLinesWorker (std::list<node_t> ns) : needles(ns) {};
	#ifdef PRINT_EXEC_TIME
	ManyLinesWorker(std::list<node_t> ns, float* ex_secs, int* execs) : 
		needles(ns), executed_secs(ex_secs), svc_executions(execs) {};
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
    PrinterWorker(std::list<node_t> ns) : ManyLinesWorker(ns) {};
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
		graph(graph), granularity(g), executed_secs(ex_secs), svc_executions(execs) {};
	#endif
		
	void* svc(void* t);
};

/* ************************** ITERATOR WORKER *****************************  */
class IteratorWorker : public ManyLinesWorker {
	public:
	IteratorWorker (std::list<node_t> ns):
		ManyLinesWorker(ns) {};
	#ifdef PRINT_EXEC_TIME
	IteratorWorker (std::list<node_t> ns, float* ex_secs, int* execs) :
		ManyLinesWorker(ns, ex_secs, execs) {};
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