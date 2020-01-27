#include "background.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//function to add a process to the list of background processes
//prints out [position in queue] [pid]
void addProcess(processes* proc, int pid, instruction* instr) {
	printf("[%i] [%i]\n", proc->length + 1, pid);

	//add new instruction to proc
	if(proc->length == 0) {
		//if background queue is empty, intitilise values
		proc->pids = (int*) malloc(sizeof(int));
		proc->cmds = (char**) malloc(sizeof(char*));

		proc->pids[0] = pid;

		//create a single char* string out of each token in instr
		char* temp = NULL;
		for(int i = 0; i < instr->numTokens - 1; i++) {
			if(i == 0) {
				temp = (char*) malloc((strlen(instr->tokens[0]) + 1) * sizeof(char));
				strcpy(temp, instr->tokens[0]);
			}
			else {
				temp = (char*) realloc(temp, ((strlen(temp) + strlen(instr->tokens[i]) + 2) * sizeof(char)));
				strcat(temp, " ");
				strcat(temp, instr->tokens[i]);
			}
		}

		//put temp into proc->cmds
		proc->cmds[0] = temp;
		proc->length++;
	}
	else {
		//otherwise, add pid and command to the end
		proc->pids = (int*) realloc(proc->pids, (proc->length) * sizeof(int));
		proc->cmds = (char**) realloc(proc->cmds, (proc->length) * sizeof(char*));

		proc->pids[proc->length] = pid;

		//create a single char* string out of each token in instr
		char* temp = NULL;
		for(int i = 0; i < instr->numTokens - 1; i++) {
			if(i == 0) {
				temp = (char*) malloc((strlen(instr->tokens[0]) + 1) * sizeof(char));
				strcpy(temp, instr->tokens[0]);
			}
			else {
				temp = (char*) realloc(temp, (strlen(temp) + strlen(instr->tokens[i]) + 2) * sizeof(char));
				strcat(temp, " ");
				strcat(temp, instr->tokens[i]);
			}
		}

		//put temp into proc->cmds
		proc->cmds[proc->length] = temp;
		proc->length++;

	}
}

//checks each process in background processes queue
//if its done running prints [position in queue]+ [cmd]
//then removes it from the queue
void checkProcesses(processes* proc) {
	int status;
	for(int i = 0; i < proc->length; i++) {
		//check if process is finished running
		int val = waitpid(proc->pids[i], &status, WNOHANG);

		if(val != 0) {
			//process has finished running
			printf("[%i]+ [%s]\n", proc->pids[i], proc->cmds[i]);

			//remove it from queue
			processes* temp;
			if(proc->length > 1) {
				temp->pids = (int*) malloc((proc->length - 1) * sizeof(int));
				temp->cmds = (char**) malloc((proc->length - 1) * sizeof(char*));
				temp->length = proc->length - 1;
			}
			else {
				free(proc->pids);
				free(proc->cmds[0]);
				free(proc->cmds);
				proc->pids = NULL;
				proc->cmds = NULL;
				proc->length = 0;
				break;
			}

			if(i == 0) {
				//first in queue
				for(int j = 0; j < temp->length; j++) {
					temp->pids[j] = proc->pids[j + 1];
					temp->cmds[j] = proc->cmds[j + 1];
				}
				free(proc->pids);
				free(proc->cmds[0]);
				free(proc->cmds);
			}
			else if(i == proc->length - 1) {
				//last in queue
				for(int j = 0; j < temp->length; j++) {
					temp->pids[j] = proc->pids[j];
					temp->cmds[j] = proc->cmds[j];
				}
				free(proc->pids);
				free(proc->cmds[temp->length]);
				free(proc->cmds);
			}
			else {
				//middle of queue
				int passed = 0;
				for(int j = 0; j < temp->length; j++) {
					if(j == i) {
						passed = 1;
					}
					if(!passed){
						temp->pids[j] = proc->pids[j];
						temp->cmds[j] = proc->cmds[j];
					}
					else {
						temp->pids[j] = proc->pids[j + 1];
						temp->cmds[j] = proc->cmds[j + 1];
					}
				}
				free(proc->pids);
				free(proc->cmds[i]);
				free(proc->cmds);
			}
			
			proc->cmds = temp->cmds;
			proc->pids = temp->pids;
			proc->length = temp->length;
			free(temp);

			//put i back one to account for missing process
			i--;
		}
	}
}
