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

// Overrides tokens and numTokens with cmd and number of command tokens
char** get_cmd(instruction* instr_ptr);

// Gets first file in tokens
char* fget_first(char** tokens, int numTokens);

// Gets last file in tokens, only for double redirection
char* fget_last(char** tokens, int numTokens);
