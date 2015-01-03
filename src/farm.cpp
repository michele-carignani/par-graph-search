/** \file: farm.cpp
	\author Michele Carignani michele.carignani@gmail.com

*/

list<string> needles;

int main(int argc, char* argv[]){
	
	/* Initialization */

    check_usage(argc, argv);

    open_graph_file(argv);
    
    load_needles_list(argv[2], needles);

	return 0;
}