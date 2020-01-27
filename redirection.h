#include <stdbool.h>

void redirection(char** cmd, int tokens);

void link_cmd_path(char** cmd, int file_desc, bool io_dir);

int parsing_rules(char** cmd, int tokens);

/* Parsing Rules Returns
 * -1 = failed
 *  0 = input
 *  1 = output
 *  2 = cmd < file > file
 *  3 = cmd > file < file
*/
