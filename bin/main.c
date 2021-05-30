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
#include <getopt.h>
#include <unistd.h>

           /* Personal Libary */
#include "printhelp.h"
#include "transfer_sequence.h"
#include "extract_sequence.h"
#include "count_char.h"
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

struct option longopts[] = {
    { "extract",     required_argument,    NULL,                'e'    },
    { "transfer",    required_argument,    NULL,                't'    },
    { "compress",    no_argument,          NULL,                'c'    },
    { "decompress",  no_argument,          NULL,                'd'    },
    { "help",        no_argument,          NULL,                'h'    },
    { "gzip",        required_argument,    NULL,                'g'    },
    { "7zip",        required_argument,    NULL,                'z'    },
    { "optimize",    required_argument,    NULL,                'o'    },
    {     0,    0,    0,    0},
};

char *l_opt_arg;  //Define a pointer to store the long option parameter

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
	printf("  Using parameter \"-h\" or \"--help\" to obtain syntax help\n");
	printf("\n");
	exit(1);
      }
    else
      {

	int c;   //Receive the prompt parameters.
	char ec; //Receive the errorous parameters.
	char output[1024]; //The filename of output file.
	const char *optstring="t:e:g:z:o:cdh";
	int index;
	int mode = 999;

	while((c = getopt_long(argc,argv,optstring,longopts,&index)) != -1)
	  {
 	    switch (c)
	     {
		case 'c':
	          mode = 1;
	          printf("****Compressing file mode****\n");
	            if (argc < 3)
		       printf("Please choose the compress or decompress mode\n");
		  break;

		case 'd':
	          mode = 0;
	          printf("****Decompressing file mode****\n");
	            if (argc < 3)
		       printf("Please choose the compress or decompress mode\n");
		  break;

	        case 't':
	          l_opt_arg = optarg;
	          transfer_sequence(l_opt_arg,output);
	          printf("Transfer \"%s\" file to \"%s\"\n", l_opt_arg, output);
	          break;

	        case 'e':
	          l_opt_arg = optarg;
	          extract_sequence(l_opt_arg,output);
	          printf("Extract \"%s\" file to \"%s\"\n", l_opt_arg, output);
	          break;

	        case 'g':
		   if ( mode > 1 )
			{
			 printf("--Parameters missing\n");
			 printf("Please choose the compression or decompression algorithm\n");
			 break;
			}
		   else
			{
			 l_opt_arg = optarg;
	          	//Gzip(l_opt_arg, mode);
	         	 printf("Compressing %s Seq to gzip...\n", l_opt_arg);
	          	break;
			}	

	        case 'z':
		   if ( mode > 1 )
			{
			 printf("--Parameters missing\n");
			 printf("Please choose the compression or decompression algorithm\n");
			 break;
			}
		   else
			{
			 l_opt_arg = optarg;
	         	 //7zip(l_opt_arg, mode);
	          	 printf("Compressing %s Seq to 7zip...\n", l_opt_arg);
	         	 break;
			}	

	        case 'o':
	          l_opt_arg = optarg;
	          //test(l_opt_arg);
	          printf("Compare different compression algorithm of your file %s \n", l_opt_arg);
	          break;

	        case 'h':
	          printhelp();
	          break;

	        /*
		  case ':':
	          printf("Parameters required\n");
	          break;
		
		case '?':
		  ec = (char)optopt;
		  printf("Parameters unknow \"%c\"\n",ec);
		*/

 	     }	

	  }

	}

}









