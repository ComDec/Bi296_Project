#include "transfer_sequence.h"
#include <stdlib.h>
#include <string.h>
void transfer_sequence(char *filename, char *output)

{
  strcpy(output,filename);
  strcat(output,".fa");
  char command1[1000] = {"sed '/^@/!d;s//>/;N' "};
  strcat(command1,filename);
  strcat(command1," >");
  system(strcat(command1,output));
  
}

