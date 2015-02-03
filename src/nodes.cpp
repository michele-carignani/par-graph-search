/** \file: nodes.cpp
*	\author: Michele Carignani <michele.carignani@gmail.com>
*/
#include <vector>
#include "graph-search.hpp"
#include "nodes.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;


/* ************************  MANY LINES EMITTER **************************** */

/* Constructor  */
ManyLinesEmitter::ManyLinesEmitter(char* pathname, int g){
    graph_file.open(pathname);
    if(!graph_file){
        error("Opening graph file");
    }
    granularity = g;
}

/* Constructor with time tracking */
#ifdef PRINT_EXEC_TIME
ManyLinesEmitter::ManyLinesEmitter(char* pathname, int g, float* ex_secs, int* execs)
    : linenum(0), granularity(g), executed_secs(ex_secs), svc_executions(execs)
    {
    graph_file.open(pathname);
    if(!graph_file){
        error("Opening graph file");
    }
}
#endif

/* Looping function */
void* ManyLinesEmitter::svc(void * t){
    int i = 0;
    char* s = new char[100];
    
#ifdef PRINT_EXEC_TIME
    /* Track execution time and excutions number */
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
#endif
    
    if(!graph_file){ return EOS; }
    
    multi_task_t* tsk = new multi_task_t(granularity);
    
    graph_file.getline(s, 100);
    while(graph_file.gcount() != 0 && i < granularity){
        linenum++;
        tsk->add_task(s, linenum);
        graph_file.getline(s, 100);
        i++;
    }
    
    /* Graph file is finished */
    if(i == 0 ) return EOS;
    
    ff_send_out(tsk);
    delete[] s;
#ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
#endif
    if(i < granularity){
        return EOS;
    }
    
    return GO_ON;
}

/* ****************************   EMITTER-NO-IO *************************     */
void EmitterNoIO::svc_end(){
#ifdef PRINT_EXEC_TIME
   //  cerr << "Emitter executed " << *executed_secs << " secs\n";
#endif
}

void* EmitterNoIO::svc(void* t){
#ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
#endif
    
    int i = 0;
    
    multi_task_t* tsk = new multi_task_t(granularity);
    while(linenum < graph->size() && i < granularity){
       linenum++;
       tsk->add_task(strdup((*graph)[linenum - 1]), linenum);
       i++;
    }
    
    if(i == 0 ) {
        delete tsk;
        return EOS;
    }
    
    ff_send_out(tsk);
    #ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
    #endif
    
    if(i < granularity){
       return EOS;
    }
    
    return GO_ON;
}

/* ******************************** MANYLINESWORKER ************************* */

void* ManyLinesWorker::svc(void* t){
    pair<node_t,node_t> found;
    
    #ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
    #endif

    multi_task_t* task = (multi_task_t*) t;
    for(int i = 0; i < task->get_count(); i++){
        single_task_t cur = task->lines[i];
        string curline (cur.line); 
        found = parse_and_check_line(&curline, &needles);
        
        if(found.first != NULL_NODE){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, cur.linenum);
            ff_send_out(res);
        }
        
        if(found.second != NULL_NODE){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, cur.linenum);
            ff_send_out(res);
        }
    }
    
    delete task;
    
    #ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
    #endif

    return (void*) GO_ON;
}

void ManyLinesWorker::svc_end(){
#ifdef PRINT_EXEC_TIME
    // cerr << "Worker " << get_my_id() << " executed " << *executed_secs << " secs\n";
#endif
}

void * Collector::svc(void * t){

    #ifdef PRINT_EXEC_TIME
    timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
    #endif

    string* res = (string*) t;
    found_nodes.push_back(*res);
    delete res;
    
    #ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
    #endif

    return GO_ON;
}

void Collector::print_res(){
    for(auto x: found_nodes){
        cout << x << "\n";
    }
}

void Collector::svc_end(){
    // print_res();
#ifdef PRINT_EXEC_TIME
    // cerr << "Collector executed " << *executed_secs << " secs\n";
#endif
}

/* ************************ ITERATOR EMITTER ******************************** */

void* IteratorEmitter::svc(void* t){
    bool finish = false;
    unsigned int end = curr + granularity;
    if(end > graph->size()){
        end  = graph->size();
        finish = true;
    }
    
    ff_send_out(new it_task_t(graph, curr, end));
    curr = end;
    
    if(finish){
        return EOS;
    }
    return GO_ON;
}

/* ************************ ITERATOR WORKER ********************************* */

void* IteratorWorker::svc(void* t){
    pair<node_t,node_t> found;
    it_task_t* task = (it_task_t*) t;
    
    for(unsigned int k = task->start; k < task->end; k++){
        string s = string((*(task->graph))[k]);
        found = parse_and_check_line( &s, &needles);
    
        if(found.first != NULL_NODE){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, k);
            ff_send_out(res);
        }

        if(found.second != NULL_NODE){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, k);
            ff_send_out(res);
        }
    
    }
    
    delete task;
    
    return GO_ON;
}

/* ********************* BYTES_EMITTER ************************************** */

