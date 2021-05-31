#include "printdoc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LINE 1024 
void printdoc(char *filename)
{
 char buf[MAX_LINE];

 FILE *fp;

 int len;

 if((fp =fopen(filename,"r")) == NULL)

 {
 perror("File lost. Please see Q&A doc to find help or Reinstall this program.\n");

 exit(1) ;

 }

 while(fgets(buf,MAX_LINE,fp) != NULL)

 {
 len= strlen(buf);

 buf[len-1] = '\0';

 printf("%s\n",buf);

 }

 

}


