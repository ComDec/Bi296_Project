#include "extract_sequence.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void extract_sequence(char *filename, char *output)
{ 
    FILE *fp1 = fopen(filename,"r"); 
              
    int CurrentIndex = 0;             
    char StrLine[1024] = {};  
    char p[1024] = {};  

    strcpy(output,filename);         
 
    strcat(output,".extract");
    
    char command[10] = {"touch "};

    system(strcat(command,output));
 
    

    FILE *fp2 = fopen(output,"rt+");

    while (!feof(fp1)) 
    { 
    
        if (CurrentIndex != 0)
        {
            fgets(StrLine,1024,fp1); 
            strcpy(p,StrLine);
	    fprintf(fp2,"%s",p);
	    memset(StrLine, 0, sizeof(StrLine));

        }
        else
        {	
	 fgets(StrLine,1024,fp1); 
	 memset(StrLine, 0, sizeof(StrLine));
         CurrentIndex++;
	}
    } 
        fclose(fp1);  
	fclose(fp2);              
}
