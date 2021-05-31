#include "creat_file.h"
#include <stdlib.h>
#include <string.h>

void creat_file(char *filename)

{
	char *command = {"touch "};
	system(strcat(command,filename));
	
}
