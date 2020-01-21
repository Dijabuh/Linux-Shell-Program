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
char** splitString(char* str, char c) {
	int len = strlen(str);
	int pos = 0;
	int num = 0;
	char** ret;
	char* temp = NULL;

	//initialize the return array
	ret = (char**) malloc(sizeof(char*));

	//loop through str
	//when we encounter c, put everything from the beggining of str up to c in temp
	//then add temp to ret
	for(int i = 0; i < len; i++) {
		if (str[i] == c && i > pos) {
			strcpy(temp, substr(str, pos, i));
			
			//now add temp to ret
			ret = (char**) realloc(ret, (num + 1) * sizeof(char));

			ret[num] = (char*) malloc((strlen(temp) + 1) * sizeof(char));

			strcpy(ret[num], temp);

			num++;
			pos = i+1;
		}
		else if (str[i] == c) {
			pos = i+1;
		}
	}

	return ret;
}
