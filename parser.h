#ifndef _PARSER_H
#define _PARSER_H
//Header file for parser


typedef struct
{
	char** tokens;
	int numTokens;
} instruction;

void getTokens(instruction* instr);
void addToken(instruction* instr_ptr, char* tok);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

#endif
