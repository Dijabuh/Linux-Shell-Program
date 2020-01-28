#include "piping.h"
#include "file_res.h"
#include "builtins.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipeParser(instruction* instr, int bg, processes* procs) {
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
	int pipefound = 0;
	for(int i = 0; i < instr->numTokens; i++) {
		if(!infound && strcmp(instr->tokens[i], "|") == 0) {
			pipefound = 1;
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
			if(pipefound) {
				//error, io in must be before first pipe
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
			else {
				//error, can only be 1 token between < and pipe
				fprintf(stderr, "Invalid use of < token\n");
				return -1;
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
			if(temp != NULL) {
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

				temp = NULL;
			}
			//if < or >, skip next token
			if(strcmp(instr->tokens[i], "<") == 0 || strcmp(instr->tokens[i], ">") == 0) {
				i++;
			}
		}
		else if (i == instr->numTokens - 1) {
			//if we are on the last token, add it to temp
			//then add temp to cmds
			if(temp == NULL) {
				//create new cmd and add it to cmds
				temp = (pipecmd*) malloc(sizeof(pipecmd));
				temp->length = 1;
				temp->fdin = 0;
				temp->fdout = 0;

				//put the first token into temp
				temp->cmd = (char**) malloc(sizeof(char*));
				temp->cmd[0] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
				strcpy(temp->cmd[0], instr->tokens[i]);
			}
			else {
				temp->cmd = (char**) realloc(temp->cmd, (temp->length + 1) * sizeof(char*));
				temp->cmd[temp->length] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
				strcpy(temp->cmd[temp->length], instr->tokens[i]);
				temp->length++;
			}
			cmds = (pipecmd**) realloc(cmds, (numCmds + 1) * sizeof(pipecmd*));
			cmds[numCmds] = temp;
			numCmds++;
		}
		else {
			//otherwise, add current token to temp
			if(temp == NULL) {
				//create new cmd and add it to cmds
				temp = (pipecmd*) malloc(sizeof(pipecmd));
				temp->length = 1;
				temp->fdin = 0;
				temp->fdout = 0;

				//put the first token into temp
				temp->cmd = (char**) malloc(sizeof(char*));
				temp->cmd[0] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
				strcpy(temp->cmd[0], instr->tokens[i]);
			}
			else {
				temp->cmd = (char**) realloc(temp->cmd, (temp->length + 1) * sizeof(char*));
				temp->cmd[temp->length] = (char*) malloc((strlen(instr->tokens[i]) + 1) * sizeof(char));
				strcpy(temp->cmd[temp->length], instr->tokens[i]);
				temp->length++;
			}
		}
	}

	//next, check that the first char* in each cmd in pipecmd is a valid file
	for(int i = 0; i < numCmds; i++) {
		char* command = cmds[i]->cmd[0];
		//if it is one of the builtins, skip it
		if(strcmp(command, "exit") == 0 ||strcmp(command, "cd") == 0 ||strcmp(command, "jobs") == 0||strcmp(command, "echo") == 0) {
			//do nothing
		}

		else if(isPath(command)){
			char* path = getAbsPathname(command);
			if(access(path, F_OK) == 0) {
				free(command);
				cmds[i]->cmd[0] = path;
			}
			else {
				printf("Command not found\n");
			}
		}
		else {
			char* path = getPath(command);
			if (path != NULL) {
				free(command);
				cmds[i]->cmd[0] = path;
			}
			else {
				printf("Command not found\n");
			}
		}
	}

	//check if we have 1 more command than pipes
	if(numPipes != numCmds - 1) {
		//error
		fprintf(stderr, "Invalid null command\n");
		//need to free up cmds and temp
		return -1;
	}

	//fill in the fd's for each command to be used for piping
	for(int i = 0; i < numPipes; i++) {
		int fd[2];
		pipe(fd);
		cmds[i]->fdout = fd[1];
		cmds[i + 1]->fdin = fd[0];
	}

	//insert null at end of each cmd
	for(int i = 0; i < numCmds; i++) {
		cmds[i]->cmd = (char**) realloc(cmds[i]->cmd, (cmds[i]->length + 1) * sizeof(char*));
		cmds[i]->cmd[cmds[i]->length] = NULL;
		cmds[i]->length++;
	}
	//run the pipe execution function and return its return value
	return pipeExec(cmds, numCmds, filein, fileout, bg, procs); 
}

int pipeExec(pipecmd** cmds, int length, char* filein, char* fileout, int bg, processes* procs) {
	for(int i = 0; i < length; i++) {
		if(i == 0) {
			//first command
			if(fork() == 0) {
				//child(command)
				//replace fd's
				if(filein != NULL) {
					//redirect stdin
					int fd = open(filein, O_RDONLY);
					close(STDIN_FILENO);
					dup(fd);
					close(fd);
				}
				//redirect stdout
				close(STDOUT_FILENO);
				dup(cmds[0]->fdout);
				close(cmds[0]->fdout);
				close(cmds[1]->fdin);

				//execute command
				char* command = cmds[i]->cmd[0];
				//check if command is one of the builtins
				if (strcmp(command, "jobs") == 0) {
					jobs(procs);
				}
				else if (strcmp(command, "echo") == 0) {
					cmds[i]->cmd[0] = NULL;
					echo(cmds[i]->cmd, cmds[i]->length);
				}
				else {
					execv(cmds[0]->cmd[0], cmds[0]->cmd);
					fprintf(stderr, "Problem executing %s\n", cmds[0]->cmd[0]);
				}
				exit(1);
			}
			else {
				//parent(shell)
				//do nothing, just loop again
			}
		}
		else if(i == length - 1) {
			//last command
			int pid = fork();
			if(pid == 0) {
				//child(command)
				//replace fds
				
				//redirect stdin
				close(STDIN_FILENO);
				dup(cmds[i]->fdin);
				close(cmds[i]->fdin);
				close(cmds[i - 1]->fdout);

				if(fileout != NULL) {
					//redirect stdout
					int fd = open(fileout, O_WRONLY);
					close(STDOUT_FILENO);
					dup(fd);
					close(fd);
				}

				//execute command
				char* command = cmds[i]->cmd[0];
				//check if command is one of the builtins
				if (strcmp(command, "jobs") == 0) {
					jobs(procs);
				}
				else if (strcmp(command, "echo") == 0) {
					cmds[i]->cmd[0] = NULL;
					echo(cmds[i]->cmd, cmds[i]->length);
				}
				else {
					execv(cmds[i]->cmd[0], cmds[i]->cmd);
					fprintf(stderr, "Problem executing %s\n", cmds[i]->cmd[0]);
				}
				exit(1);
			}
			else {
				//parent(shell)
				//close each of the fd's for each command to be used for piping
				for(int i = 0; i < length-1; i++) {
					close(cmds[i]->fdout);
					close(cmds[i + 1]->fdin);
				}

				if(!bg) {
					//if its not running in the background, simply wait until last command is dont running
					int status;
					waitpid(pid, &status, 0);
					return -1;
				}
				//if it is running in the background, return the pid
				return pid;
			}
		}
		else {
			//command in middle
			if(fork() == 0) {
				//child(command)
				//replace fds
				close(STDIN_FILENO);
				dup(cmds[i]->fdin);
				close(cmds[i]->fdin);
				close(cmds[i - 1]->fdout);

				close(STDOUT_FILENO);
				dup(cmds[i]->fdout);
				close(cmds[i]->fdout);
				close(cmds[i + 1]->fdin);

				//execute command
				char* command = cmds[i]->cmd[0];
				//check if command is one of the builtins
				if (strcmp(command, "jobs") == 0) {
					jobs(procs);
				}
				else if (strcmp(command, "echo") == 0) {
					cmds[i]->cmd[0] = NULL;
					echo(cmds[i]->cmd, cmds[i]->length);
				}
				else {
					execv(cmds[i]->cmd[0], cmds[i]->cmd);
					fprintf(stderr, "Problem executing %s\n", cmds[i]->cmd[0]);
				}
				exit(1);
			}
			else {
				//parent(shell)
				//do nothing, just loop again
				close(cmds[i]->fdin);
				close(cmds[i-1]->fdout);
			}
		}
	}
}
