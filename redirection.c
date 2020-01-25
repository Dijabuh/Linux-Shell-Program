#include <fcntl.h>
#include <unistd.h>

#include "redirection.h"
#include "execution.h"

void redirection(char** cmd){
	/*
	bool input_redir = (redirection_symbol == '<');
	int file_desc;

	if(input_redir)
		file_desc = open(f_input, O_RDONLY);
	else
		file_desc = open(f_input, O_WRONLY);

	link_cmd_path(cmd, file_desc, input_redir);
	*/
}

void link_cmd_path(char** cmd, int file_desc, bool io_dir){

	if(fork() == 0){
		
		if(io_dir)
			close(STDIN_FILENO);
		else
			close(STDOUT_FILENO);

		dup(file_desc);
		close(file_desc);
		
		// Execute process here
		execute(cmd);
	} else{
		// Parent?
		close(file_desc);
	}
}
