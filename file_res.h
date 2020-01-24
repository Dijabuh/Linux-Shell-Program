//Header for file_res.c

typedef struct
{
	char** parts;
	int numParts;
} pathparts;

int isPath(char* str);
char* substr(char* str, int start, int end);
void splitString(pathparts* ret, char* str, char* c);
char* getAbsPathname(char* str);
char* getAbsPathname(char* str);
char* getPath(char* cmd);
