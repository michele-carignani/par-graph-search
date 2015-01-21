/** \file: nodes.cpp
*	\author: Michele Carignani <michele.carignani@gmail.com>
*/
#include <vector>
#include "graph-search.hpp"
#include "nodes.hpp"
#include "utils.hpp"

using namespace std;
using namespace ff;

/*        MANY LINES EMITTER      */
ManyLinesEmitter::ManyLinesEmitter(char* pathname, int g){
    graph_file.open(pathname);
    if(!graph_file){
        error("Opening graph file");
    }
    granularity = g;
}

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

void ManyLinesEmitter::svc_end(){
#ifdef PRINT_EXEC_TIME
    // cerr << "Emitter executed " << *executed_secs << " secs\n";
#endif
}

void* ManyLinesEmitter::svc(void * t){
#ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
#endif
    if(!graph_file){
            return EOS;
    }
    int i = 0;
    char* s = new char[100];
    multi_task_t* tsk = new multi_task_t(granularity);
    graph_file.getline(s, 100);
    while(graph_file.gcount() != 0 && i < granularity){
        linenum++;
        tsk->add_task(string(s), linenum);
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

/*     EMITTERNOIO     */
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
    while(linenum < graph.size() && i < granularity){
       linenum++;       
       tsk->add_task(string(graph[linenum - 1]), linenum);
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

/* MANYLINESWORKER */
void* ManyLinesWorker::svc(void* t){
    #ifdef PRINT_EXEC_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    *svc_executions = *svc_executions + 1;
    #endif

    multi_task_t* task = (multi_task_t*) t;
    for(int i = 0; i < task->get_count(); i++){
        parse_line(task->lines[i]);
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

void ManyLinesWorker::parse_line(single_task_t task){
    /* Ignore comments */
    if(task.line[0] == '#') return;
   
    string first = task.line.substr(0, task.line.find("\t"));
    string second = task.line.substr(task.line.find("\t") + 1, task.line.find("\r") - 1 - task.line.find("\t"));
   
    list<string>::iterator it;
    for(it = needles.begin(); it != needles.end(); it++){
        #ifdef DEBUG
         cout << "Comparo " << (*it) << " e " << first << " e " << second << "\n";
        #endif
       if((*it).compare(first) == 0){
           found_node(task.linenum, *it);
       }
       if((*it).compare(second) == 0){
           found_node(task.linenum, *it);
       }
   }
}

void ManyLinesWorker::found_node(int linenum, std::string needle){
    char* line_num = new char[50];
    
    sprintf(line_num, "%d :",linenum);
    string* res = new string (line_num);
    res->append(needle);

    ff_send_out(res);
    delete[] line_num;
}

void PrinterWorker::found_node(int linenum, string needle){
    cout << needle << " : " << linenum << "\n";
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
    print_res();
#ifdef PRINT_EXEC_TIME
    // cerr << "Collector executed " << *executed_secs << " secs\n";
#endif
}
