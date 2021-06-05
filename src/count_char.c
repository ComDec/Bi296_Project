#include "count_char.h"
#include <stdio.h>
int count_char(char *filename)
{
FILE *fp;
fp = fopen(filename,"rt+");
int num=0;

while(!feof(fp))
  {
    if( fgetc(fp))
	num++;
  }



fclose(fp);

return num;
}
