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
#include "printdoc.h"
#include "transfer_sequence.h"
#include "extract_sequence.h"
#include "count_char.h"
#include "creat_file.h"
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
    { "huffman",     required_argument,    NULL,                'm'    },
    { "MFC",         required_argument,    NULL,                'f'    },
    { "optimize",    required_argument,    NULL,                'o'    },
    { "parallel",    required_argument,    NULL,                'p'    },
    { "speed",       required_argument,    NULL,                's'    },
    { "version",     no_argument,          NULL,                'v'    },
    {     0,    0,    0,    0},
};

char *l_opt_arg;  //Define a pointer to store the long option parameter

int speed = 2;

char parallel[20] = {"2"};

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
	const char *optstring="t:e:m:z:f:o:p:s:cvdh";
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
		       printf("Please choose the compress or decompress algorithm\n");
		  break;

		case 'd':
	          mode = 0;
	          printf("****Decompressing file mode****\n");
	            if (argc < 3)
		       printf("Please choose the compress or decompress algorithm\n");
		  break;

		case 's':
	          speed = atoi(optarg);
		  break;

		case 'p':
		  
		  strcpy(parallel,optarg);

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

	        case 'f':

		   if ( mode == 999 )
			{
			 printf("--Parameters missing\n");
			 printf("Please choose the compression or decompression algorithm\n");
			 break;
			}
		   else
			{
			   l_opt_arg = optarg;

				
			   if (mode == 1)
			      {

				printf("Compressing %s Seq via MFC method...\n", l_opt_arg);

				printf("**Mode %d**\n",speed);
			
				printf("**Parallel progression %s**\n",parallel);
				
				char command[300] = {"./MFCompressC "};
				
				switch(speed)
				   {
 			   	     case '1':
					strcat(command," -1");
					break;
				     case '2':
					strcat(command," -2");
					break;
				     case '3':
					strcat(command," -3");
					break;
				     default:
					strcat(command," -2");
					break;
				   }

				if (strcmp(parallel,"2") != 0)

				   {
 				      strcat(command," -t ");
				      strcat(command,parallel);
				   }
				   	
				strcat(command," ");
				system(strcat(command,l_opt_arg));

			      }
	          	   else if (mode == 0)
			      {
				char command[300] = {"./MFCompressD"};	
				
				printf("Decompressing %s Seq via MFC method...\n", l_opt_arg);

				printf("**Parallel progression %s**\n",parallel);

				if (strcmp(parallel,"2") != 0)

				   {
 				      strcat(command," -t ");
				      strcat(command,parallel);
				   }
				   	
				strcat(command," ");	

				//strcat(command," -o ");

				//printf("%s",command);

				system(strcat(command,l_opt_arg));	        
			      }
	         	 
	          	break;
			}	

	        case 'm':
		     if ( mode == 999 )
			{
			 printf("--Parameters missing\n");
			 printf("Please choose the compression or decompression algorithm\n");
			 break;
			}
		     else
			{
			   l_opt_arg = optarg;

			   char binfile[200] = {};

			   char tablefile[200] = {};
				
		           strcpy(binfile,l_opt_arg);
				
			   strcpy(tablefile,l_opt_arg);

			   if (mode == 1)
			      {

				printf("Compressing %s Seq via huffman method...\n", l_opt_arg);
				
				strcat(binfile,"_bin");

				strcat(tablefile,"_table");

				creat_file(binfile);  //Creat output file

			        creat_file(tablefile);

				char command[300] = {"./encode -f "};

				system(strcat(command,l_opt_arg));
			      }
	          	   else if (mode == 0)
			      {
			        char command[300] = {"./decode -f "};
				char outfile[200] = {};
				
				strcpy(outfile,l_opt_arg);

			        strcat(binfile,"_bin");
				strcat(tablefile,"_table");
				strcat(outfile,"decode");
	
				printf("Decompressing %s Seq via huffman method...\n", l_opt_arg);

				strcat(command,binfile);
				strcat(command," -t ");
				strcat(command,tablefile);
				strcat(command," -o ");
				strcat(command,outfile);
				
				system(command);
			      }
	         	 
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
	          printdoc("../doc/help.txt");
	          break;

	        case 'v':
	          printdoc("../doc/version.txt");
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









