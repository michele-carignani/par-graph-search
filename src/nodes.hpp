/** \file:nodes.hpp
    \author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>


#ifndef _NODES_HPP
#define _NODES_HPP

class ManyLinesEmitter : public ff::ff_node {
    private:
    int linenum = 0;
    int granularity = 20;
    std::ifstream graph_file;

   public:
   void * svc(void  * t);
   ManyLinesEmitter(char* pathname, int g) {
        graph_file.open(pathname);
        granularity = g;
    };

};

class ManyLinesWorker : public ff::ff_node {
    private:
    std::list<std::string>* needles;

    public:
    ManyLinesWorker (std::list<std::string>* ns) : needles(ns) {};
    void * svc(void* t);
    void parse_line(single_task_t task);
    virtual void found_node(void* t, std::string needle){return;};

};

class PrinterWorker : public ManyLinesWorker {

	public:
    PrinterWorker(std::list<std::string>* ns) : ManyLinesWorker(ns) { 
    };
    void found_node(void* t, std::string needle);
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
    public:
    std::list<int> found_nodes;

    void * svc(void * t);
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
