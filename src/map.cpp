/** \file: map.cpp
	\author Michele Carignani michele.carignani@gmail.com
*/

#include <ff/parallel_for.hpp>

#include "graph-search.hpp"

using namespace ff;

ifstream graph_file;
ifstream needle_file;

void found_node(string node, int line){
    cout << "Trovato nodo " << node << " alla riga " << line "\n"; 
}

void check_line(char* line, int linenum){
    if(line[0] == '#') return;
    string first = line.substr(0, task.line.find("\t"));
    string second = line.substr(task.line.find("\t") + 1, task.line.find("\r") - 1 - task.line.find("\t"));
    list<string>::iterator it;
    for(it = needles.begin(); it != needles.end(); it++){
        // cout << "Comparo " << (*it) << " e " << first << " e " << second << "\n";
        if((*it).compare(first) == 0){
            found_node(*it, linenum );
        }
        if((*it).compare(second) == 0){
            found_node(*it, linenum);
        }
    }
}

int main(int argc, char* argv){

    ParallelFor pf;
    pf.parallel_for(0,100, [&A](const long i)){
       readLine();
       check_line();
    }
    
	
	return 0;
}
