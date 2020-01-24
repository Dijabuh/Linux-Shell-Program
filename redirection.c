#include <unistd.h>

#include "execution.h"

void redirection(char** cmd){

	if(/*redirection symbol*/){
		redirect_input(cmd);
	} else{
		redirect_output(cmd);
	}
}

void redirect_input(char* f_input, char* cmd){
	
	// Opens file to take input from
	int file_desc = open(f_input);

	if(fork() == 0){
		close(STDIN_FILENO);
		dup(file_desc);
		close(file_desc);
		// Execute process here
	} else{
		// Parent?
		close(file_desc);
	}
}

void redirect_output(char** cmd){


}
