/** \file:nodes.hpp
    \author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <string>
#include <iostream>

class ManyLinesEmitter : public ff::ff_node {
    private:
    int linenum = 0;
    int granularity = 20;

    ManyLinesEmitter(int g) : granularity(g) {};

   public:
   void * svc(void  * t);

};

class ManyLinesWorker : public ff::ff_node {
    public:
    void * svc(void* t);    
    void parse_line(single_task_t task);
    virtual void found_node(void* t, std::string needle){return;};

};

class PrinterWorker : public ManyLinesWorker {
	public:
    void found_node(void* t, std::string needle);
};

class HasherWorker : public ManyLinesWorker {
    
    std::unordered_multimap<int, int>* results;

	public:
    HasherWorker(std::unordered_multimap<int, int>* rs) : results(rs) {};
    void found_node(void* t, std::string needle);
};

class FileReader: public ff::ff_node {

    int first = 0;
    std::ifstream* graph_file;

    public:

    int svc_init();     
    FileReader(std::ifstream gf): graph_file(gf) {};
    bytes_task_t* svc(string_task_t* t);
};


class Graph_parser : ff_node_t<string_task_t> {
    std::string_task_t* svc();
};