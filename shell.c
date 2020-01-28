// COP 4610 Project 1: Implementating a Shell
// Garett Anderson, Daniel Brown

#include "parser.h"
#include "file_res.h"
#include "builtins.h"
#include "execution.h"
#include "piping.h"
#include "redirection.h"
#include "background.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){
	int numInstructionsRun = 0;
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	processes procs;
	procs.pids = NULL;
	procs.cmds = NULL;
	procs.length = 0;

	while (1) {
		char* user = getenv("USER");
		char* machine = getenv("MACHINE");
		char* pwd = getenv("PWD");
		printf("%s@%s : %s > ", user, machine, pwd);

		getTokens(&instr);

		//order of executing
		//Remove beggining & if it exists
		//Remove trailing & if it exists and flag for background execution
		//Make sure there are no & in the middle of the tokens
		//Check for | and run piping execution if its there
		//Check for < or > and run io redirection if its there
		//Otherwise run execution code we currently have
	
		//First check if 1st token is &
		if(strcmp(instr.tokens[0], "&") == 0) {
			//if so, remove it and ignore
			int newlen = instr.numTokens - 1;
			char** newtokens = (char**) malloc(newlen * sizeof(char*));
			for(int i = 0; i < newlen; i++) {
				newtokens[i] = instr.tokens[i + 1];
			}

			//free the first token and the old tokens array
			free(instr.tokens[0]);
			free(instr.tokens);

			//assign instr.tokens to newtokens
			instr.tokens = newtokens;
			instr.numTokens = newlen;
		}

		//Next check if last token is &
		int backgroundexec = 0;
		if(strcmp(instr.tokens[instr.numTokens - 1], "&") == 0) {
			//if so, remove it and flag for background execution
			backgroundexec = 1;

			int newlen = instr.numTokens - 1;
			char** newtokens = (char**) malloc(newlen * sizeof(char*));
			for(int i = 0; i < newlen; i++) {
				newtokens[i] = instr.tokens[i];
			}

			//free the first token and the old tokens array
			free(instr.tokens[instr.numTokens - 1]);
			free(instr.tokens);

			//assign instr.tokens to newtokens
			instr.tokens = newtokens;
			instr.numTokens = newlen;
		}
	
		int error = 0;
		//Then check that there are not anymore & in the list of tokens
		for(int i = 0; i < instr.numTokens; i++) {
			if(strchr(instr.tokens[i], '&') != NULL) {
				fprintf(stderr, "Invalid user of & token\n");
				clearInstruction(&instr);
				error = 1;
			}
		}

		if(error) {
			continue;
		}

		int piping = 0;
		//Check if tokens has a |
		//If so, run the piping execution function
		for(int i = 0; i < instr.numTokens; i++) {
			if(strchr(instr.tokens[i], '|') != NULL) {
				//run piping function
				piping = 1;
				int pid = 0;
				pid = pipeParser(&instr, backgroundexec, &procs);
				if(pid != -1) {
					addNull(&instr);
					addProcess(&procs, pid, &instr);
				}
				numInstructionsRun++;
				break;
			}
		}

		if(piping) {
			clearInstruction(&instr);
			continue;
		}

		int ioredir = 0;
		//Check if tokens has a < or >
		//If so, run the io redirection execution function
		for(int i = 0; i < instr.numTokens; i++) {
			if(strchr(instr.tokens[i], '<') != NULL || strchr(instr.tokens[i], '>') != NULL) {
				//run io redirection function
				redirection(&instr, backgroundexec);
				ioredir = 1;
				numInstructionsRun++;
				break;
			}
		}

		if(ioredir) {
			clearInstruction(&instr);
			continue;
		}

		//add in null to end of tokens
		addNull(&instr);

		//Not piping or io redirection, so just execute the command
		//Need to add in background execution
		char* command = instr.tokens[0];

		//check if command is one of the builtins
		if(strcmp(command, "exit") == 0) {
			EXIT(numInstructionsRun, &procs);
		}
		else if (strcmp(command, "cd") == 0) {
			if(instr.numTokens == 2) {
				cd(instr.tokens[1]);
				numInstructionsRun++;
			}
			else if(instr.numTokens == 1) {
				cd(NULL);
				numInstructionsRun++;
			}
			else {
				printf("Too many arguements for cd\n");
			}
		}
		else if (strcmp(command, "jobs") == 0) {
			jobs(&procs);
			numInstructionsRun++;
		}
		else if (strcmp(command, "echo") == 0) {
			instr.tokens[0] = NULL;
			echo(instr.tokens, instr.numTokens);
			numInstructionsRun++;
		}

		//Check if first token, ie the command is a path
		//if it is, resolve the pathname, check if it exists, if it does run it
		//need to switch from using instr.tokens[0] to a char*
		//instr.tokens[0] could be an &
		else if(isPath(command)){
			char* path = getAbsPathname(command);
			if(access(path, F_OK) == 0) {
				free(instr.tokens[0]);
				instr.tokens[0] = path;
				if(backgroundexec) {
					int pid = execute_bckgrnd(instr.tokens);	
					addProcess(&procs, pid, &instr);
				}
				else {
					execute(instr.tokens);	
				}
				numInstructionsRun++;
			}
			else {
				printf("Command not found\n");
			}
		}
		else {
			char* path = getPath(command);
			if (path != NULL) {
				free(instr.tokens[0]);
				instr.tokens[0] = path;
				if(backgroundexec) {
					int pid = execute_bckgrnd(instr.tokens);	
					addProcess(&procs, pid, &instr);
				}
				else {
					execute(instr.tokens);	
				}
				numInstructionsRun++;
			}
			else {
				printf("Command not found\n");
			}
		}
		clearInstruction(&instr);
		checkProcesses(&procs);
	}

	return 0;
}
