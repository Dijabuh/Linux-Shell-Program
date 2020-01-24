// COP 4610 Project 1: Implementating a Shell
// Garett Anderson, Daniel Brown

#include "parser.h"
#include "file_res.h"
#include "builtins.h"
#include "execution.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){
	int numInstructionsRun = 0;
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
			EXIT(numInstructionsRun);
		}
		else if (strcmp(command, "cd") == 0) {
			if(instr.numTokens == 3) {
				cd(instr.tokens[1]);
				numInstructionsRun++;
			}
			else if(instr.numTokens == 2) {
				cd(NULL);
				numInstructionsRun++;
			}
			else {
				printf("Too many arguements for cd\n");
			}
		}
		else if (strcmp(command, "jobs") == 0) {
			numInstructionsRun++;
		}
		else if (strcmp(command, "echo") == 0) {
			numInstructionsRun++;
		}
		
		//Check if first token, ie the command is a path
		//if it is, resolve the pathname, check if it exists, if it does run it
		//need to switch from using instr.tokens[0] to a char*
		//instr.tokens[0] could be an &
		else if(isPath(command)){
			printf("Is a path\n");
			char* path = getAbsPathname(command);
			printf("%s\n", path);
			if(access(path, F_OK) == 0) {
				printf("Full path: %s\n", path);
				instr.tokens[0] = realloc(instr.tokens[0], ((strlen(path) + 1) * sizeof(char)));
				strcpy(instr.tokens[0], path);
				execute(instr.tokens);	
				numInstructionsRun++;
			}
		}
		else {
			printf("Is a command\n");
			char* cmdpath = getPath(command);
			if (cmdpath != NULL) {
				printf("Full path: %s\n", cmdpath);
				instr.tokens[0] = realloc(instr.tokens[0], ((strlen(cmdpath) + 1) * sizeof(char)));
				strcpy(instr.tokens[0], cmdpath);
				execute(instr.tokens);	
				numInstructionsRun++;
			}
			else {
				printf("Command not found\n");
			}
		}
		clearInstruction(&instr);
	}

	return 0;
}
