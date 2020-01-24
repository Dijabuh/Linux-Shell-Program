#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"

void EXIT(int commands){

	printf("Exiting now! Executed %d commands!", commands);

	exit(EXIT_SUCCESS);
}
