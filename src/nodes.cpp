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
    linenum = 0;
    granularity = 20; 
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
    cout << "LETTO: " << s << "\n";
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
        found = parse_and_check_line(cur.line, needles, needles_count);
        
        if(!found.first.is_null()){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, cur.linenum);
            ff_send_out(res);
        }
        
        if(!found.second.is_null()){
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

/* ***************************** COLLECTOR ********************************** */

void * Collector::svc(void * t){

    #ifdef PRINT_EXEC_TIME
    timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
    #endif
   
    pair<node_t,int>* res = (pair<node_t,int>*) t;
    char res_str[50];
    char* str_val = res->first.str();
    sprintf(res_str,"%d : %s", res->second, str_val);
    free(str_val);
    found_nodes.push_back(string(res_str));
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
#ifdef PRINT_RESULTS
    print_res();
#endif
}

/* ************************ ITERATOR EMITTER ******************************** */

void* IteratorEmitter::svc(void* t){

#ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
#endif
    
    bool finish = false;
    unsigned int last = curr + granularity;
    if(last > graph->size()){
        last  = graph->size();
        finish = true;
    }
    
#ifdef ON_MIC
    it_task_t* tskbuf = (it_task_t*) __mm_alloc(sizeof(it_task_t), 64);
#else
    it_task_t* tskbuf = (it_task_t*) malloc(sizeof(it_task_t));
#endif
    it_task_t* tsk = new (tskbuf) it_task_t(graph, curr, last);
    ff_send_out(tsk);
    curr = last;
    
    if(finish){
        return EOS;
    }
    
    #ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
    #endif

    return GO_ON;
}

/* ************************ ITERATOR WORKER ********************************* */

void* IteratorWorker::svc(void* t){
    pair<node_t,node_t> found;
    it_task_t* task = (it_task_t*) t;
    
    #ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
    #endif
    
    for(unsigned int k = task->start; k < task->end; k++){
        found = parse_and_check_line( (*(task->graph))[k], needles, needles_count);
    
        if(!found.first.is_null()){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, k+1);
            ff_send_out(res);
        }

        if(!found.second.is_null()){
            pair<node_t,int>* res =  new pair<node_t,int>(found.second, k+1);
            ff_send_out(res);
        }
    
    }
    
    delete task;
    
    #ifdef PRINT_EXEC_TIME
    clock_gettime(CLOCK_REALTIME, &end);
    *executed_secs += elapsed_time_secs(start, end);
    #endif
    
    return GO_ON;
}

/* ********************* BYTES_EMITTER ************************************** */

