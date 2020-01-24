//Resolves a given pathname to an absolute pathname

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
	char* temp = NULL;

	//initialize the return array
	ret->parts = (char**) malloc(sizeof(char*));
	ret->numParts = 0;

	temp = strtok(str, "/");
	ret->parts = (char**) realloc(ret->parts, (ret->numParts + 1) * sizeof(char*));
	(ret->parts)[ret->numParts] = (char*) malloc((strlen(temp) + 1) * sizeof(char));
	strcpy((ret->parts)[ret->numParts], temp);
	(ret->numParts)++;

	//keep looping until we've gone through every part of the path
	while(temp != NULL) {
		temp = strtok(NULL, "/");
		if (temp != NULL) {
			ret->parts = (char**) realloc(ret->parts, (ret->numParts + 1) * sizeof(char*));
			(ret->parts)[ret->numParts] = (char*) malloc((strlen(temp) + 1) * sizeof(char));
			strcpy((ret->parts)[ret->numParts], temp);
			(ret->numParts)++;
		}
	}

	free(temp);
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
		absLen = strlen(getenv("HOME"));
		absPath = (char*) malloc((absLen + 1) * sizeof(char));
		strcpy(absPath, getenv("HOME"));
		pos = 1;
	}
	else {
		//we start at pwd
		absLen = strlen(getenv("PWD"));
		absPath = (char*) malloc((absLen + 1) * sizeof(char));
		strcpy(absPath, getenv("PWD"));
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
			absLen = strrchr(absPath, '/') - absPath;
			if (absLen == 0) {
				absLen = 1;
			}
			absPath = (char*) realloc(absPath, (absLen + 1) * sizeof(char));
			//add null character
			absPath[absLen] = '\0';
		}
		else {
			//otherwise, add the current part to the absolute pathname
			if (absLen == 1) {
				//in this case, we are at root
				absLen += newlen;
				absPath = (char*) realloc(absPath, (absLen + 1) * sizeof(char));
				strcat(absPath, curPart);
			}
			else {
				absLen += newlen + 1;
				absPath = (char*) realloc(absPath, (absLen + 1) * sizeof(char));
				strcat(absPath, "/");
				strcat(absPath, curPart);
			}
		}

	}

	free((split.parts));

	return absPath;
}

//Retrieves the path of a given command from the $PATH variable
//Returns NULL if the file doesnt exist
char* getPath(char* cmd) {
	int pathVarLen = strlen(getenv("PATH"));
	char* pathVar = (char*) malloc((pathVarLen + 1) * sizeof(char));
	strcpy(pathVar, getenv("PATH"));
	
	int cmdLen = strlen(cmd);

	//Split the $PATH variable up into a list of pathnames seperated by :
	pathparts pathList;
	splitString(&pathList, pathVar, ':');

	//loop through all paths in list
	//add cmd onto them
	//if the file exists, return it
	for(int i = 0; i < pathList.numParts; i++) {
		int pathLen = strlen(pathList.parts[i]);
		char* curPath = (char*) malloc((pathLen + cmdLen + 1) * sizeof(char));
		
		//get current path and add cmd onto it
		strcpy(curPath, pathList.parts[i]);
		strcat(curPath, cmd);

		//check if it exists
		if(access(curPath, F_OK) == 0) {
			return curPath;
		}
	}

	return NULL;
}

