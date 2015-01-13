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
    float executed_secs = 0; /** Keeps track of execution cpu time  fo the node */

   public:
   void * svc(void  * t);
   
   /**
    * 
    * @param pathname Pathname of the input file
    * @param g Size of the task to be emitted. Default is 20
    */
   ManyLinesEmitter(char* pathname, int g);
   
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
        float executed_secs = 0;
        
    public:
        /**
         * 
         * @param graph Vector of lines of the edgelist file
         * @param g Size of the multi_task_t task
         */
        EmitterNoIO(std::vector<char*> graph, int g = 20):
            linenum(0), granularity(g), graph(graph){};
            
        ~EmitterNoIO() {};
        void* svc(void * t);
        void svc_end();
};

class ManyLinesWorker : public ff::ff_node {
    private:
    std::list<std::string> needles;
    float executed_secs = 0;
    
    public:
    ManyLinesWorker (std::list<std::string> ns) : needles(ns) {};
    ~ManyLinesWorker () {};
    void * svc(void* t);
    void parse_line(single_task_t task);
    virtual void found_node(int linenum, std::string needle);
    void svc_end();

};

class PrinterWorker : public ManyLinesWorker {
    public:
    PrinterWorker(std::list<std::string> ns) : ManyLinesWorker(ns) {};
    void found_node(int linenum, std::string needle);
};

/*
class HasherWorker : public ManyLinesWorker {
    private:

	public:
    std::unordered_multimap<int, int>* results;

    HasherWorker(std::unordered_multimap<int, int> rs) : results(rs) {};

    void found_node(void* t, std::string needle);
};
*/

class Collector : public ff::ff_node {
    float executed_secs = 0;
    public:
    std::list<std::string> found_nodes;

    void * svc(void * t);
    void svc_end();
    void print_res();
};

#endif // _NODES_HPP

#ifdef NODEF

class FileReader: public ff::ff_node {

    int first = 0;
    std::ifstream graph_file;

    public:

    int svc_init();
    FileReader(std::ifstream gf): graph_file(gf) {};
    bytes_task_t* svc(string_task_t* t);
};


class Graph_parser : ff_node_t<string_task_t> {
    string_task_t* svc();
};

#endif
