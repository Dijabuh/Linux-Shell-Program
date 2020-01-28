#ifndef _BACKGROUND_H
#define _BACKGROUND_H
#include "parser.h"

typedef struct {
	int* pids;
	char** cmds;
	int length;
} processes;

void addProcess(processes* proc, int pid, instruction* instr);
void checkProcesses(processes* proc);

#endif
