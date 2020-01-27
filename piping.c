#include "piping.h"
#include "file_res.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int pipeParser(instruction* instr, int bg) {
	int numPipes = 0;
	//first get number of pipes
	for(int i = 0; i < instr->numTokens; i++) {
		if(strcmp(instr->tokens[i], "|") == 0) {
			numPipes++;
		}
	}

	//Next check for io redirection in
	char* filein = NULL;
	int infound = 0;
	for(int i = 0; i < instr->numTokens; i++) {
		if(!infound && strcmp(instr->tokens[i], "|") == 0) {
			//error, io in must be before first pipe
			fprintf(stderr, "Invalid use of < token\n");
			return -1;
		}
		else if(infound && strcmp(instr->tokens[i], "<") == 0) {
			//error, no more than 1 io redirection in
			fprintf(stderr, "Invalid use of < token\n");
			free(filein);
			return -1;
		}
		else if(!infound && strcmp(instr->tokens[i], "<") == 0) {
			if(i == 0) {
				//error, < cant be first token
				fprintf(stderr, "Invalid use of < token\n");
				return -1;
			}
			//check if 2 tokens ahead is a pipe
			if(instr->numTokens -2 >= i && strcmp(instr->tokens[i + 2], "|") == 0) {
				filein =  getAbsPathname(instr->tokens[i + 1]);
				//check that the file exists
				if(access(filein, F_OK) != 0) {
					fprintf(stderr, "File %s does not exist\n", filein);
					free(filein);
					return -1;
				}
			}
		}
	}
	
	//Then check for io redirection out
	char* fileout = NULL;
	int outfound = 0;
	for(int i = 0; i < instr->numTokens; i++) {
		if(outfound && strcmp(instr->tokens[i], "|") == 0) {
			//error, io out must be after last pipe
			fprintf(stderr, "Invalid use of > token\n");
			return -1;
		}
		else if(outfound && strcmp(instr->tokens[i], ">") == 0) {
			//error, no more than 1 io redirection out
			fprintf(stderr, "Invalid use of > token\n");
			return -1;
		}
		else if(!infound && strcmp(instr->tokens[i], ">") == 0) {
			//check if it is 2nd to last token
			if(instr->numTokens - 2 == i) {
				fileout =  getAbsPathname(instr->tokens[i + 1]);
				//check that the file exists
				if(access(fileout, F_OK) != 0) {
					fprintf(stderr, "File %s does not exist\n", fileout);
					free(fileout);
					return -1;
				}
			}
		}
	}

	//next group together commands and their arguements in pipecmd struct
	pipecmd** cmds = NULL;
	cmds = (pipecmd**) malloc(sizeof(pipecmd*));
	int numCmds = 0;

	pipecmd* temp = NULL;
	for(int i = 0; i < instr->numTokens; i++) {
		if(i == 0) {
			//create first pipecmd
			temp = (pipecmd*) malloc(sizeof(pipecmd));
			temp->length = 1;
			temp->fdin = 0;
			temp->fdout = 0;

			//put the first token into temp
			temp->cmd = (char**) malloc(sizeof(char*));
			temp->cmd[0] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
			strcpy(temp->cmd[0], instr->tokens[i]);
		}
		else if(strcmp(instr->tokens[i], "<") == 0 || strcmp(instr->tokens[i], ">") == 0 || strcmp(instr->tokens[i], "|") == 0) {
			//make sure this isnt the last token
			if(i == instr->numTokens - 1) {
				//error, last token cant be < > or |
				fprintf(stderr, "Invalid use of %s token\n", instr->tokens[i]);
				//need to free up cmds and temp
				return -1;
			}
			//otherwise, add current temp to cmds and start a new one
			cmds = (pipecmd**) realloc(cmds, (numCmds + 1) * sizeof(pipecmd*));
			cmds[numCmds] = temp;
			numCmds++;

			//start new pipecmd
			temp = (pipecmd*) malloc(sizeof(pipecmd));
			temp->length = 1;
			temp->fdin = 0;
			temp->fdout = 0;

			//put the first token into temp
			temp->cmd = (char**) malloc(sizeof(char*));
			temp->cmd[0] = (char*) malloc((strlen(instr->tokens[i + 1]) + 1) * sizeof(char));
			strcpy(temp->cmd[0], instr->tokens[i + 1]);
			i++;
		}
		else {
			//otherwise, add current token to temp
			temp->cmd = (char**) realloc(temp->cmd, (temp->length + 1) * sizeof(char*));
			temp->cmd[temp->length] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
			strcpy(temp->cmd[temp->length], instr->tokens[i]);
			temp->length++;
		}
	}

}
