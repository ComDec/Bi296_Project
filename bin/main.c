/*
 * CompressDNA -- This is a program for DNA sequence compressing.
 * Copyright(C) 2021-2022 WX-WZY-GRC Group
 * The compression algorithm was developed by Zhongyi Wang, Ruichu Gu.
 * We utilize some codes from GNU, GitHub, and CSDN.
 * We develop this little tools for Bioinformatic work.
 * 
 * See the license_msg below and the file COPYING for the software license.
 * See the file algorithm.doc for the compression algorithms and file formats.
*/

static char  *license_msg[] = {
"   Copyright (C) 2021-2022 WX-WZY-GRC Group",
"   This program is free software; you can redistribute it and/or modify",
"   it under the terms of the GNU General Public License as published by",
"   the Free Software Foundation; either version 2, or (at your option)",
"   any later version.",
"",
"   This program is distributed in the hope that it will be useful,",
"   but WITHOUT ANY WARRANTY; without even the implied warranty of",
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"   GNU General Public License for more details.",
"",
"   You should have received a copy of the GNU General Public License",
"   along with this program; if not, write to the Free Software",
"   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.",
0};

           /* Library from C99 standard */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

           /* Personal Libary */
#include "printhelp.h"
/*
#include "Convert.h"
#include "FiletoSeq.h"
#include "7z.h"

/*
*************************

add your own library here.


*************************
*/




           /* Local Functions*/
/*
*************************


add your function declare here.


*************************
*/

           /* Local Variables*/
/*
*************************


add your variables here.


*************************
*/

int main(int argc, char **argv)
{
    if (argc == 1)
      {
	printf("\n");
	printf("\n");
	printf("  Please checkout syntax in README.md file\n");
	printf("\n");
	printf("  Using parameter \"-h\" or \"help\" to obtain syntax help\n");
	printf("\n");
	exit(1);
      }
    else
      {
	//printf("%s\n",argv[1]);
        if (strcmp(argv[1],"-h") ==0 || strcmp (argv[1],"help") ==0)
	   { 
	     printhelp();
	     exit(1);
	   }
        else if (strcmp(argv[1],"-c") ==0 || strcmp (argv[1],"convert") ==0)
	   { 
	     //FiletoSeq("./argv[2]","argv[2]");
	     printf("Converting file to Seq....\n");
	     exit(1);
	   }
        else if (strcmp(argv[1],"-gz") ==0 || strcmp (argv[1],"gzip") ==0)
	   { 
	     //Gzip("./argv[2]","argv[2]");
	     printf("Compressing Seq to gzip...\n");
	     exit(1);
	   }
        else if (strcmp(argv[1],"-7z") ==0 || strcmp (argv[1],"7zip") ==0)
	   { 
    	     //7zip("./argv[2]","argv[2]");
	     printf("Compressing Seq to 7zip...\n");
	     exit(1);
	   }
	else if (strcmp(argv[1],"-t") ==0 || strcmp (argv[1],"test") ==0)
	   { 
    	     //test("./argv[2]","argv[2]");
	     printf("Compare different compression algorithm of your file\n");
	     exit(1);
	   }
	else
	   {
	     printf("\n");
	     printf(" Parameter is elusive. Please check your input and manual.\n");
	     printf("\n");
	     printhelp();
	   }

      }




}









