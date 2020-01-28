#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "builtins.h"
#include "file_res.h"

void EXIT(int commands){

	printf("Exiting now! Executed %d commands!\n", commands);

	exit(EXIT_SUCCESS);
}

//Function to change the current directory
//Returns 0 if successful, -1 otherwise
int cd(char* path) {
	if(path == NULL) {
		//if no arguement supplied, change directory to $HOME
		chdir(getenv("HOME"));
		setenv("PWD", getenv("HOME"), 1);
		return 0;
	}
	else {
		char* newpath = getAbsPathname(path);
		//otherwise, check if directory exists, and if it does, change to it
		DIR* dir = opendir(newpath);
		if(dir != NULL) {
			//directory exists
			chdir(newpath);
			setenv("PWD", newpath, 1);
		}
		else {
			//failed for some reason
			perror("Couldnt change directory");
			//printf("Could not change directory: %s\n", strerror(errno));
			return -1;
		}
	}
}

void echo(char** args, int length) {
	for(int i = 0; i < length; i++) {
		if(args[i] != NULL) {
			if(args[i][0] == '$') {
				//env var
				char* sub = substr(args[i], 1, strlen(args[i]));
				printf("%s ", getenv(sub));
				free(sub);
			}
			else {
				printf("%s ", args[i]);
			}
		}
	}
	printf("\n");
}
