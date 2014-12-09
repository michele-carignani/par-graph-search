#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include <istream>
#include <fstream>
#include <string>
#include <iostream>

#include "graph-search.hpp"

using namespace ff;
using namespace std;

ifstream graph_file;


/**  Performs I/O operations and dispatches
*	 the bytes read to the next stage in the pipeline
*/
class fileReader: ff_node_t<string_task_t, bytes_task_t> {

	int first = 0;

	int svc_init(){
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
	}; 	

	bytes_task_t* svc(bytes_task_t *t){
		
		graph_file.read(buf + (first * (BUFSIZE / 2)), BUFSIZE / 2);
		if(graph_file.eofbit){
			return EOS;
		}

		if(!graph_file){
			perror("reading graph file");
		}

		ff_send_out(new bytes_task_t(first *(BUFSIZE / 2), graph_file.gcount()));

		if(first) first = 0;
		else first = 1;
		
		return EOS;
	}
};


/** Receives a pointer to the read bytes,
*	scan them to find newlines characters
*	and dispatches sets of lines to the farm.
*/
class lineSplitter: ff_node_t<bytes_task_t, string_task_t> {
	int task_first_char = -1, task_last_newline = -1, task_size;
	
	char left_to_read[LEFT_BUF_SIZE];
	int left_end = 0;

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

class graph_parser : ff_node_t<string_task_t> {
	string_task_t* svc(){
		// ff_send_out()
		return GO_ON;
	}
};

int main(int argc, char* argv[]){

	if(argc < 2){
		cout << "Usage: " << argv[0] << " <graph-file> <node>\n";
		exit(0);
	}

	char* filein = argv[1];
	graph_file.open(filein);

	if(!graph_file){
		perror("opening graph file");
		exit(0);
	}

	// todo parametrico
	// std::vector<ff_node*> W = {new graph_parser, new graph_parser};

	ff_pipe<string_task_t> pipe(new fileReader, new lineSplitter ); // , new ff_farm<>(W));
	pipe.wrap_around();
	pipe.cleanup_nodes();
	if(pipe.run_and_wait_end()<0) error("running pipe");

	graph_file.close();
	return 0;
}

