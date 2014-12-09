/**
* 	
*	\author Michele Carignani michele.carignani@gmail.com
*/

// todo: different configurations
// 		1. trova se il nodo c'è o no
//		1. trova i link con quel nodo

// todo: directed or undericted graphs or both?



char* read_n_lines(infile){
	char* mybuf[100];
	read()
	return buf;
}

void emitter(int infile){
	char* lines;
	char* mybuf;
	while(lines != NULL){
		lines = read_n_lines(infile);
		send(lines, get_worker(i));
	}
}


void worker(){
	int i;

	while(!emitter_finished){
		
		lines_num = receive_from(emitter, &lines);
		
		for(i = 0; i < lines_num; i++){

			if( n = contains(lines[i], needles, &result) ){
				
				if(conf("node")){
					for(int j = 0; j < n; j++){
						send(result[j], gatherer);
					}
				} else {
					// signal also which is the needle
					send(lines[i], gatherer);
				}
			}
		}
	}
}

void gatherer(){
	while(!workers_finished){
		receive_from(any_worker, &data);
		if(conf("node")){
			insert_unique(results, data);
		} else {
			insert_hash(results, wanted(data), value(data));
		}
	}
	printout(results);
}

int main(int argc, char* argv[]){

	if(argc < 2){
		usage();
		exit(0);
	}

	char* infile_name = argv[1]
	int infile = open(infile_name);
	
	map( emitter, worker, gatherer );

}