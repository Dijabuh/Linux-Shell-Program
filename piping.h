//Header file for pipe execution
#include "parser.h"
#include "background.h"

typedef struct
{
	char** cmd;
	int length;
	int fdin;
	int fdout;
} pipecmd;

int pipeParser(instruction* instr, int bg, processes* procs);
int pipeExec(pipecmd** cmds, int length, char* filein, char* fileout, int bg, processes* procs);
