// COP 4610 Project 1: Implementating a Shell
// Garett Anderson, Daniel Brown

#include "parser.h"
#include "file_res.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;



	while (1) {
		char* user = getenv("USER");
		char* machine = getenv("MACHINE");
		char* pwd = getenv("PWD");
		printf("%s@%s : %s > ", user, machine, pwd);

		getTokens(&instr);
		//do something with the instruction here	
		clearInstruction(&instr);
	}

	return 0;
}
