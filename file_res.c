//Resolves a given pathname to an absolute pathname

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_res.h"

//Checks if a given string is a pathname
//Returns true if it has a / in it
bool isPath(char* str) {
	if (strchr(str, '/') == NULL) {
		return false;
	}
	return true;
}

//function to get a substring of a given string
//Substring includes start and is up to but not including end
char* substr(char* str, int start, int end) {
	int len = end - start;
	int pos = 0;
	char* ret = (char*) malloc((len + 1) * sizeof(char));
	for (int i = start; i < end; i++) {
		ret[pos] = str[i];
		pos++;
	}

	return ret;
}

//Splits up a string into an array of strings seperated by a given char
void splitString(pathparts* ret, char* str, char c) {
	int len = strlen(str);
	int pos = 0;
	char* temp = NULL;

	//initialize the return array
	ret->parts = (char**) malloc(sizeof(char*));
	ret->numParts = 0;

	//loop through str
	//when we encounter c, put everything from the beggining of str up to c in temp
	//then add temp to ret
	for(int i = 0; i < len; i++) {
		if (str[i] == c && i > pos) {
			strcpy(temp, substr(str, pos, i));
			
			//now add temp to ret
			ret->parts = (char**) realloc(ret, (ret->numParts + 1) * sizeof(char));

			(ret->parts)[ret->numParts] = (char*) malloc((strlen(temp) + 1) * sizeof(char));

			strcpy((ret->parts)[ret->numParts], temp);

			(ret->numParts)++;

			pos = i+1;
		}
		else if (str[i] == c) {
			pos = i+1;
		}
	}
}

//Returns the absolute pathname of a given path
char* getAbsPathname(char* str) {
	int len = strlen(str);
	int absLen = 0;
	char* absPath = NULL;
	pathparts split;
	int pos = 0;

	//first check first char of str
	if (str[0] == '/') {
		//then we are dealing with an absolute pathname
		absLen = 1;
		absPath = (char*) malloc((absLen + 1) * sizeof(char));
		strcpy(absPath, "/");
	}
	else if (str[0] == '~' || (len > 4 && strcmp("$HOME", substr(str, 0, 5)) == 0)) {
		//then we start at $HOME
		absLen = strlen(getenv("$HOME"));
		absPath = (char*) malloc((absLen + 1) * sizeof(char));
		strcpy(absPath, getenv("$HOME"));
		pos = 1;
	}
	else {
		//we start at pwd
		absLen = strlen(getenv("$PWD"));
		absPath = (char*) malloc((absLen + 1) * sizeof(char));
		strcpy(absPath, getenv("$PWD"));
	}

	//get split string
	splitString(&split, str, '/');

	//check each part of the path and add it on to the abs pathname
	for(int i = pos; i < split.numParts; i++) {
		char* curPart;
		int newlen = strlen(split.parts[i]);
		curPart = (char*) malloc((newlen + 1) * sizeof(char));
		strcpy(curPart, split.parts[i]);

		if (strcmp(curPart, ".") == 0) {
			//in this case, we ignore and do nothing
		}
		else if (strcmp(curPart, "..") == 0) {
			//in this case, we remove one part from our absolute path name
			//if we are at root, do nothing
			absLen = strrchr(absPath, '/') - str;
			absPath = (char*) realloc(absPath, (absLen) * sizeof(char));
		}
		else {
			//otherwise, add the current part to the absolute pathname
			absLen += newlen;
			absPath = (char*) realloc(absPath, (absLen + 2) * sizeof(char));
			strcat(absPath, "/");
			strcat(absPath, curPart);
		}

	}

	return absPath;
}
