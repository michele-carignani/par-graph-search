/** \file: nodes.hpp
*    \author: Michele Carignani michele.carignani@gmail.com
*/

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include "graph-search.hpp"

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
    int linenum = 0; /** Keeps track of number of lines read */
    int granularity = 20; /** Size of the tasks emitted */
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
   void svc_end();

};

/**
 *  A class describing an emitter node that takes
 *  the input data from a vector of strings (without performing
 *  IO operations).
 */
class EmitterNoIO : public ff::ff_node {
    private:
        unsigned int linenum;
        int granularity;
        std::vector<char*> graph;
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
        EmitterNoIO(std::vector<char*> graph, int g = 20):
            linenum(0), granularity(g), graph(graph){};
		
		#ifdef PRINT_EXEC_TIME
		EmitterNoIO(std::vector<char*> graph, int g, float* ex_secs, int* execs) : 
			linenum(0), granularity(g), graph(graph),
			executed_secs(ex_secs), svc_executions(execs) {};
		#endif
            
        ~EmitterNoIO() {};
        void* svc(void * t);
        void svc_end();
};


/** 
 *  Defines a worker that takes a multi_task_t task and 
 *  loop over the lines contained. Every time a node is 
 *  found, the found_node() method is called.
 */
class ManyLinesWorker : public ff::ff_node {
    private:
    std::list<std::string> needles; /** List of nodes to be looked for */
#ifdef PRINT_EXEC_TIME
    float* executed_secs;
	int* svc_executions;
#endif
    
    public:
    ManyLinesWorker (std::list<std::string> ns) : needles(ns) {};
	#ifdef PRINT_EXEC_TIME
	ManyLinesWorker(std::list<std::string> ns, float* ex_secs, int* execs) : 
		needles(ns), executed_secs(ex_secs), svc_executions(execs) {};
	#endif
    ~ManyLinesWorker () {};
    void * svc(void* t);
    void parse_line(single_task_t task);
    virtual void found_node(int linenum, std::string needle);
    void svc_end();

};

/** A class for a worker that simply prints to stdout
 *  when a node is founde: for each node found, a line is
 *  printed with node ID and line number.
 */
class PrinterWorker : public ManyLinesWorker {
    public:
    PrinterWorker(std::list<std::string> ns) : ManyLinesWorker(ns) {};
    void found_node(int linenum, std::string needle);
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

#endif // _NODES_HPP