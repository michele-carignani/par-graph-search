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
        error("Errore apertura graph file");
    }
    granularity = g;
}

void ManyLinesEmitter::svc_end(){
#ifdef PRINT_EXEC_TIME
    cerr << "Emitter executed " << executed_secs << " secs\n";
#endif
}

void* ManyLinesEmitter::svc(void * t){
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
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
       if(i == 0 ) return EOS;
       ff_send_out(tsk);
       delete[] s;
       clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
       executed_secs += elapsed_time_secs(start, end);
       if(i < granularity){
           return EOS;
        }
        return GO_ON;
}

/* */
void EmitterNoIO::svc_end(){
#ifdef PRINT_EXEC_TIME
    cerr << "Emitter executed " << executed_secs << " secs\n";
#endif
}

void* EmitterNoIO::svc(void* t){
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    int i = 0;
    multi_task_t* tsk = new multi_task_t(granularity);
    while(linenum < graph.size() && i < granularity){
       linenum++;
       // cout <<"lol " << graph[linenum] << "\n";
       tsk->add_task(string(graph[linenum - 1]), linenum);
       i++;
    }
    if(i == 0 ) {
        delete tsk;
        return EOS;
    }
    ff_send_out(tsk);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    executed_secs += elapsed_time_secs(start, end);
    if(i < granularity){
       return EOS;
    }
    return GO_ON;
}

/* */

void* ManyLinesWorker::svc(void* t){
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
   multi_task_t* task = (multi_task_t*) t;
   for(int i = 0; i < task->get_count(); i++){
       parse_line(task->lines[i]);
   }
   delete task;
   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
   executed_secs += elapsed_time_secs(start, end);
   return (void*) GO_ON;
}

void ManyLinesWorker::svc_end(){
#ifdef PRINT_EXEC_TIME
    cerr << "Worker " << get_my_id() << " executed " << executed_secs << " secs\n";
#endif
}

void ManyLinesWorker::parse_line(single_task_t task){
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
    
    #ifdef DEBUG
    // cout << "Mando " << res->c_str() << "\n";
    #endif

    ff_send_out(res);
    delete[] line_num;
}

void PrinterWorker::found_node(int linenum, string needle){
    cout << "Trovato " << needle << " alla riga " << linenum << "\n";
}

void * Collector::svc(void * t){
    timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    string* res = (string*) t;
    // cout << "ricevuto " << res->c_str() << "\n";
    // if(! found_nodes.contans(res)){}
    found_nodes.push_back(*res);
    delete res;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    executed_secs += elapsed_time_secs(start, end);
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
    cerr << "Collector executed " << executed_secs << " secs\n";
#endif
}

#ifdef DONT

/*
    *************************************************************
		NO STD IO VERSIONS
    *************************************************************
*/

/**  Performs I/O operations and dispatches
*	 the bytes read to the next stage in the pipeline
*/
int FileReader::svc_init(){
	char* buf = new char[BUFSIZE];
	graph_file.read(buf, BUFSIZE / 2);

	if(graph_file.eofbit){
		return 0;
	}

	if(!graph_file){
		perror("reading graph file");
		return 0;
	}

	ff_send_out(new bytes_task_t(0, graph_file.gcount()));
	first = 1;
}

bytes_task_t* FileReader::svc(string_task_t* t){

	graph_file->read(buf + (first * (BUFSIZE / 2)), BUFSIZE / 2);
	if(graph_file.eofbit){
		return EOS;
	}

	if(!(*graph_file)){
		perror("reading graph file");
	}

	ff_send_out(new bytes_task_t(first *(BUFSIZE / 2), graph_file.gcount()));

	if(first) first = 0;
	else first = 1;

	return EOS;
}

/** Receives a pointer to the read bytes,
*	scan them to find newlines characters
*	and dispatches sets of lines to the farm.
*/
class lineSplitter: public ff_node_t<bytes_task_t, string_task_t> {

	int task_first_char = -1, task_last_newline = -1, task_size;
	char left_to_read[LEFT_BUF_SIZE];
	int left_end = 0;
	char* buf = new char[BUFSIZE];

	string_task_t* svc(bytes_task_t *rt){
		int t_num = 0, i = 0;
		char* task_string = NULL;


		task_first_char = rt->start;

		// read the entire read buffer
		while(i < rt->size){
			// there is room for task
			while(i < TASK_SIZE){
				// line is finished
				if(buf[(rt->start)+i] == '\n'){
					// save ref to last new line
					task_last_newline = rt->start + i;
				}
				i++;
			}

			if(left_end != 0){
				strncpy(task_string, left_to_read, left_end);
			}

			// create and send task
			task_size = task_last_newline - task_first_char + 1;
			task_string = (char*)malloc(sizeof(char) * task_size);
			strncpy(task_string+left_end, buf+task_first_char, task_size);

			ff_send_out(new string_task_t(task_string, strlen(task_string)));

			// update scanning state and continue
			task_first_char = task_last_newline + 1;
			if(left_end != 0){
				left_end = 0;
			}

			i++;
		}

		// save locally the bytes that were not included in any tasks
		int left_end = rt->size - task_last_newline;
		strncpy(
			left_to_read,
			buf + task_last_newline+1,
			(LEFT_BUF_SIZE > left_end ? LEFT_BUF_SIZE : left_end)
		);

		// send feedback to the first stage
		// to signal that the buffer can be freed
		// todo ff_send_out_to();
		return EOS;
	}

};

string_task_t* Graph_parser::svc(){
	// ff_send_out()
	return GO_ON;
}
#endif
