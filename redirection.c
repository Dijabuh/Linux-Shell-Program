#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "redirection.h"
#include "execution.h"

void redirection(instruction* instr_ptr){

	// if(!parsing_rules(instr_ptr->cmds, tokens)){ return; } // Failed parsing rules

	/*
	bool input_redir = (redirection_symbol == '<');
	int file_desc;

	if(input_redir){
		if(access(f_input, F_OK) != -1)
			file_desc = open(f_input, O_RDONLY);
		else
			printf("Unable to open file %s\n", f_input);
	} else
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

int parsing_rules(char** cmd, int tokens){
	
	/* Rules
	 1. 1 less <>'s than cmds
	 2. No more than 2 <>'s
	 3. If 2 <>, must alternate
	 4. Can't start or end with <>'s
	 5. Must alternate <>'s and cmds
	*/

	int cmd_counter = 0;
	int redir_counter = 0;

	char input[] = "<";
	char output[] = ">";
	
	// Tracks if on cmd/<> for rule 5
	bool alt = true;	// true = don't accept <>
	
	// Keeps track of 2 <>'s for rule 3
	int multi_redir = -1; // -1=none, 0=<, 1=>

	// Checks for rule 4
	if(strcmp(cmd[0], input) == 0 || strcmp(cmd[0], output) == 0)
		return -1;

	for(int i = 0; i < tokens; ++i){
		
		if(strcmp(cmd[i], input) == 0 || strcmp(cmd[i], output) == 0){
			// Checks for rule 2
			if(redir_counter > 1){ return -1; }

			if(!alt){
				// Checks for rule 3
				if(multi_redir == 0)
					if(strcmp(cmd[i], input) == 0){ 
						return -1; 
					}
				else if(multi_redir == 1)
					if(strcmp(cmd[i], output) == 0){ 
						return -1;
					}
				else{
					// Sets which <> called
					if(strcmp(cmd[i], input) == 0){
						multi_redir = 0;
					}
					else{ multi_redir = 1; }
				}

				alt = true; // tracks for 5
				++redir_counter;
				// Here bc cmd may have flags/args after
				++cmd_counter;
			} else{
				return -1; // violates 5
			}
		} else{
			alt = false; // command currently
		}
	}

	// Checking if ended with command string
	if(!alt){ ++cmd_counter; }
	else{ return -1; } // violates 4

	if(redir_counter + 1 != cmd_counter){ return -1; } // violates 1

	if(redir_counter == 1){ return multi_redir; } // returns < or >
	else if(redir_counter == 2){
		// Returns either for < > or > <
		if(multi_redir == 0){ return 2; }
		else if(multi_redir == 1){ return 3; }
		else{ return -1; }
	} else{ return -1; }
}
