#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "graph-search.hpp"

char pre[BUFSIZE/4];
char token[BUFSIZE/8];
char* needle = NULL;
int line = 1;
char toprint[50];
int isComment = 0;
int j = 0;
int found = 0;

inline void printLineNum(char c){
	sprintf(toprint, "l: %d\n", (c == '\n') ? line - 1 : line);
	write(STDOUT_FILENO, toprint, strlen(toprint));
}

int readChar(char c){
	switch(c){
	
		case '\r':
			break;	

		case '#':
			isComment = 1;
			break;

		case '\n':
			line++;
			if(isComment){
				isComment = 0;
				break;
			}
		
		case '\t':
		case ' ':
			if(!isComment){
				token[j] = '\0';
				if(strcmp(token, needle) == 0){
					// printf("C: %d L: %d T: %s\n", c, (c != '\n') ? line : line - 1, token);
					//printLineNum(c);
					found++;
				}
			}
			// memcpy(token, 0, BUFSIZE/8);
			j = 0;
			token[j] = '\0';
 			break;

		default:
			if(!isComment){

				token[j] = c;
				j++;
			}
	}
}

int main(int argc, char** argv){
	int f = open(argv[1], O_RDONLY);
	needle = argv[2];
	int n, i;

	n = read(f, buf, BUFSIZE);
	while(n){
		for(i = 0; i < n; i++){
			readChar(buf[i]);
		}
		n = read(f, buf, BUFSIZE);
	}
	
	sprintf(toprint, "%d\n", found);
	write(STDOUT_FILENO, toprint, strlen(toprint));

}

