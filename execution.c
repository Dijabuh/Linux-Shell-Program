#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execution.h"

void execute(char** cmd){
	
	int status;
	pid_t pid = fork();

	if(pid == -1){
		exit(1);
	} else if(pid == 0){
		execv(cmd[0], cmd);
		printf("Problem executing %s\n", cmd[0]);
		exit(1);
	} else{
		waitpid(pid, &status, 0);
	}
}

void execute_bckgrnd(char** cmd){

	int status;
	pid_t pid = fork();

	if(pid == -1){
		exit(1);
	} else if(pid == 0){
		execv(cmd[0], cmd);
		printf("Problem executing %s\n", cmd[0]);
		exit(1);
	} else{
		waitpid(pid, &status, WNOHANG);
	}
}
