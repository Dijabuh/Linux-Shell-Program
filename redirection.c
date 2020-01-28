#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execution.h"
#include "file_res.h"
#include "redirection.h"

int redirection(instruction* instr_ptr, bool background){

	int no_bckgrnd = -1;
	int pid;
	// Returns if failed parsing rules
	int redir_case = parsing_rules(instr_ptr->tokens, instr_ptr->numTokens);
	if(redir_case == -1){ return no_bckgrnd; } // violation of redirection rules

	int file_desc;
	int file_desc2;

	char** cmd = NULL;
	char* file = fget_first(instr_ptr->tokens, instr_ptr->numTokens);
	char* file2 = NULL;

	if(redir_case == 0 || redir_case == 1){

		if(access(file, F_OK) != -1 && redir_case == 0)
			file_desc = open(file, O_RDONLY);
		else if(redir_case == 1){
			// Opens a new file for output if non-existent
			if(access(file, F_OK) != -1) remove(file);

			FILE* fp = fopen(file, "w");
			fclose(fp);
			file_desc = open(file, O_WRONLY);
		} else{
			printf("Unable to open file %s\n", file);
			free(file);
			return no_bckgrnd;
		}
	
		cmd = get_cmd(instr_ptr);

		pid = single_redirection(cmd, file_desc, (
			redir_case == 0 ? true : false), background);

	} else if(redir_case == 2 || redir_case == 3){

		file2 = fget_last(instr_ptr->tokens, instr_ptr->numTokens);

		if(access(file, F_OK) != -1 && redir_case == 2){
			file_desc = open(file, O_RDONLY);

			if(access(file2, F_OK) != -1) remove(file2);

			FILE* fp = fopen(file2, "w");
			fclose(fp);
			file_desc2 = open(file2, O_WRONLY);
		}
		else if(access(file2, F_OK) != -1 && redir_case == 3){
			file_desc2 = open(file2, O_RDONLY);

			if(access(file, F_OK) != -1) remove(file);

			FILE* fp = fopen(file, "w");
			fclose(fp);
			file_desc = open(file, O_WRONLY);
		}
		else{
			printf("Unable to open file %s\n", file);
			free(file);
			free(file2);
			return no_bckgrnd;
		}
		cmd = get_cmd(instr_ptr);

		pid = double_redirection(cmd, file_desc, file_desc2, (
			redir_case == 2 ? true : false), background);
	}

	free(cmd);
	if(background)
		return pid;
	return no_bckgrnd;
}

int single_redirection(char** cmd, int file_desc,
bool direction, bool background){

	int status;
	pid_t pid = fork();

	if(pid == -1) return pid;
	else if(pid == 0){
		
		if(direction)
			close(STDIN_FILENO);
		else
			close(STDOUT_FILENO);

		dup(file_desc);
		close(file_desc);
			
		execv(cmd[0], cmd);
		printf("Problem executing %s\n", cmd[0]);
		return -1;
	} else{
		if(background)
			waitpid(pid, &status, WNOHANG);
		else
			waitpid(pid, &status, 0);
		
		close(file_desc);
	}
	return pid;
}

int double_redirection(char** cmd, int file_desc1,
int file_desc2, bool direction, bool background){

	int status;
	pid_t pid = fork();
	
	if(pid == -1) return pid;
	else if(pid == 0){
		
		if(direction){
			close(STDIN_FILENO);
			dup(file_desc1);
			close(file_desc1);
			close(STDOUT_FILENO);
			dup(file_desc2);
			close(file_desc2);
		} else{
			close(STDOUT_FILENO);
			dup(file_desc1);
			close(file_desc1);
			close(STDIN_FILENO);
			dup(file_desc2);
			close(file_desc2);
		}
		
		execv(cmd[0], cmd);
		printf("Problem executing %s\n", cmd[0]);
		return -1;
	} else{
		if(background)
			waitpid(pid, &status, WNOHANG);
		else
			waitpid(pid, &status, 0);
		
		close(file_desc1);
		close(file_desc2);
	}
	return pid;
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
	if(strcmp(cmd[0], input) == 0 || strcmp(cmd[0], output) == 0){
		printf("Invalid null command.\n");
		return -1;
	}

	for(int i = 0; i < tokens; ++i){
		
		if(strcmp(cmd[i], input) == 0 || strcmp(cmd[i], output) == 0){
			// Checks for rule 2
			if(redir_counter > 1){
				printf("Ambiguous redirect.\n");
				return -1;
			}

			if(!alt){
				// Checks for rule 3
				if(multi_redir == 0){
					if(strcmp(cmd[i], input) == 0){
						printf("Ambiguous input redirect.\n");
						return -1; 
					}
				} else if(multi_redir == 1){
					if(strcmp(cmd[i], output) == 0){ 
						printf("Ambiguous output redirect.\n");
						return -1;
					}
				} else{
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
				printf("Missing name for redirect.\n");
				return -1; // violates 5
			}
		} else{
			alt = false; // command currently
		}
	}

	// Checking if ended with command string
	if(!alt){ ++cmd_counter; }
	else{
		printf("Missing name for redirect.\n");
		return -1; 
	} // violates 4

	if(redir_counter + 1 != cmd_counter){
		printf("Amiguous redirect.\n");	
		return -1; 
	} // violates 1

	if(redir_counter == 1){ return multi_redir; } // returns < or >
	else if(redir_counter == 2){
		// Returns either for < > or > <
		if(multi_redir == 0){ return 2; }
		else if(multi_redir == 1){ return 3; }
		else{ return -1; }
	} else{ return -1; }
}

char** get_cmd(instruction* instr_ptr){

	int end_cmd = -1;

	for(int i = 0; i < instr_ptr->numTokens; ++i){
		
		if(strcmp(instr_ptr->tokens[i], "<") == 0 || strcmp(
		instr_ptr->tokens[i], ">") == 0){
			end_cmd = i;
			break;
		}
	}

	if(end_cmd == -1) return NULL;
	
	char** cmd = (char**) malloc(sizeof(char*) * (end_cmd+1));
	for(int i = 0; i < end_cmd; ++i){
		cmd[i] = (char*) malloc((strlen(instr_ptr->tokens[i]) + 1) * sizeof(char));
		strcpy(cmd[i], instr_ptr->tokens[i]);
	}
	cmd[end_cmd] = NULL;
	
	char* path = getPath(cmd[0]);
	if (path != NULL){
		free(cmd[0]);
		cmd[0] = path;
	} else
		return NULL;

	return cmd;
}

char* fget_first(char** tokens, int numTokens){
	
	int i;

	for(i = 0; i < numTokens; ++i){
		if(strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0)
			break;
	}
	
	char* file = (char*) malloc(sizeof(tokens[i+1]));
	strcpy(file, tokens[i+1]);
	return file;
}

char* fget_last(char** tokens, int numTokens){

	bool firstRedir = false;
	int i;

	for(i = 0; i < numTokens; ++i){
		
		if(strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0){
			if(firstRedir)
				break;
			else
				firstRedir = true;
		}
	}
	
	char* file = (char*) malloc(sizeof(tokens[i+1]));
	strcpy(file, tokens[i+1]);
	return file;
}
