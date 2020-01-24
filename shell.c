// COP 4610 Project 1: Implementating a Shell
// Garett Anderson, Daniel Brown

#include "parser.h"
#include "file_res.h"
#include "builtins.h"
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
	
		char* command = instr.tokens[0];
		//check if command is one of the builtins
		if(strcmp(command, "exit") == 0) {

		}
		else if (strcmp(command, "cd") == 0) {
			if(instr.numTokens == 3) {
				cd(instr.tokens[1]);
			}
			else if(instr.numTokens == 2) {
				cd(NULL);
			}
			else {
				printf("Too many arguements for cd\n");
			}
		}
		else if (strcmp(command, "jobs") == 0) {

		}
		else if (strcmp(command, "echo") == 0) {

		}

		else if(isPath(instr.tokens[0])){
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
