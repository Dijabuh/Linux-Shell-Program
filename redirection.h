#include <stdbool.h>

#include "parser.h"

void redirection(instruction* instr_ptr, bool background);

void single_redirection(char** cmd, int file_desc,
	bool direction, bool background);

void double_redirection(char** cmd, int file_desc1,
	int file_desc2, bool direction, bool background);

int parsing_rules(char** cmd, int tokens);

/* Parsing Rules Returns
 * -1 = failed
 *  0 = input
 *  1 = output
 *  2 = cmd < file > file
 *  3 = cmd > file < file
*/

void get_cmd(char** tokens, int numTokens, char** cmd);

// Gets first file in tokens
void fget_first(char** tokens, int numTokens, char* file);

// Gets last file in tokens, only for double redirection
void fget_last(char** tokens, int numTokens, char* file);
