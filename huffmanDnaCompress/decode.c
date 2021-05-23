#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "huffman_table.h"
#include "coder.h"

int main (int argc, char *argv[])
{
	int c;
	char *binFileName = NULL;
	char *tableFileName = NULL;
	char *outFileName = NULL;
	char *binaryInput = NULL;
	HuffmanTable *huffmanTable = NULL;
	FILE *outFile = NULL;
	size_t len = 0;
	char *dnaSeq = NULL;
	while((c = getopt(argc, argv,"hf:t:o:")) != -1){
		switch(c)
		{
			case 'f':
				binFileName = argv[2];
				break;
			case 't':
				tableFileName = argv[2];
				break;
			case 'o':
				outFileName = argv[2];
				break;
			case 'h':
			default:
				printf("Uso: decode -f binaryFile -t huffmanTableFile -o outputFile\n");
				return -1;
		}
	}
	if(binFileName == NULL || tableFileName == NULL || outFileName == NULL){
		printf("Uso: decode -f binaryFile -t huffmanTableFile -o outputFile\n");
	}
	FILE *input = fopen(binFileName, "r");
	if(input == NULL){
		printf("Error abriendo fichero %s\n", binFileName);
		return -1;
	}

	getline(&binaryInput, &len, input);
	//binaryInput[strlen(binaryInput) - 1] = '\0';
	huffmanTable = importHuffmanTableFromFile(tableFileName);
	printHuffmanTable(huffmanTable);

	if(huffmanTable == NULL){
		printf("Error importando tabla de Huffman del fichero %s\n", tableFileName);
		free(binaryInput);
		return -1;
	}

	dnaSeq = getOriginalMessage(binaryInput, huffmanTable);

	outFile = fopen(outFileName, "w");
	if(outFile == NULL){
		printf("Error abriendo fichero %s\n", outFileName);
		free(binaryInput);
		free(dnaSeq);
		freeHuffmanTable(huffmanTable);
		return -1;
	}
	fputs(dnaSeq, outFile);
	fclose(outFile);

	free(binaryInput);
	free(dnaSeq);
	freeHuffmanTable(huffmanTable);
}
