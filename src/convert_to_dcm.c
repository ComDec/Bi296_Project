#include "dcm.h"

void transfer(char *input[])
{
  if (input == NULL)
	printf("Input file lost.\n");
  else
   {
    FILE * fp;
    fopen(fp, input,"rb+");
    
    fclose(fp);    

   }



}
