// COP 4610 Project 1: Implementating a Shell
// Garett Anderson, Daniel Brown

#include "parser.h"
#include "file_res.h"
#include "execution.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;



	while (1) {
		char* user = getenv("USER");
		char* machine = getenv("MACHINE");
		char* pwd = getenv("PWD");
		printf("%s@%s : %s > ", user, machine, pwd);

		getTokens(&instr);
		//do something with the instruction here	
	
		if(isPath(instr.tokens[0])){
			printf("Is a path\n");
			char* path = getAbsPathname(instr.tokens[0]);
			printf("%s\n", path);
			free(path);
		}
		else {
			printf("Is a command\n");
			char* cmdpath = getPath(instr.tokens[0]);
			if (cmdpath != NULL) {
				printf("Full path: %s\n", cmdpath);
				strcpy(instr.tokens[0], cmdpath);
				execute(instr.tokens);	
			}
			else {
				printf("Command not found\n");
			}
			free(cmdpath);
		}

		clearInstruction(&instr);
	}

	return 0;
}
