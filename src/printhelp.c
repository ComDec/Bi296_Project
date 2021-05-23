#include "printhelp.h"

void printhelp()
{
	
	FILE *pfile = fopen("./doc/help.txt", "rb");
	if (pfile == NULL) {
		printf("open file failed!\n");
		exit(0);
	}

	
	fseek(pfile,0, SEEK_END);
	int file_length = ftell(pfile);
	rewind(pfile);

	
	int text_length = file_length * sizeof(char) + 1;
	char* text_buffer = (char*)malloc(text_length);
	memset(text_buffer, 0, text_length);

	
	int read_length = fread(text_buffer, 1, file_length, pfile);
	if (read_length != file_length) {
		printf("read file failed!\n");
		exit(0);
	}

	
	printf(text_buffer);
	free(text_buffer);
	fclose(pfile);
}


