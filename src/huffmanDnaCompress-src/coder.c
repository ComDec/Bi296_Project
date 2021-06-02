#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coder.h"
#include "string_utils.h"

char* getBinaryString(char *input, HuffmanTable *huffmanTable)
{
	int i, j;
	int *positions = NULL;
	char *symbol = NULL;
	int symbolLen;
	char *code = NULL;
	int codeLen;
	int n = 0;

	char* binaryArrayAux[strlen(input)];
	for (i = 0; i < strlen(input); i++){
		binaryArrayAux[i] = NULL;
	}
	char* binaryArray = NULL;
	int binaryArrayLen = 0;

	char *inputCopy = strndup(input, strlen(input));

	for (i = 0; i < huffmanTable->length; i++){
		code = huffmanTable->huffmanPair[i]->code;
		codeLen = strlen(code);
		symbol = huffmanTable->huffmanPair[i]->symbol;
		symbolLen = strlen(symbol);
		positions = searchSubstring(inputCopy, symbol, &n, !OVERLAPPED_SEARCH);
		for(j = 0; j < n; j++){
			binaryArrayAux[positions[j]] = strndup(code, codeLen);
			binaryArrayLen += codeLen;
			
			/* Eliminamos de la copia de la entrada los simbolos ya procesados, sustituyendolos por '_' */
			memset(inputCopy + positions[j],'_', symbolLen);
		}
		
		free(positions);
	}

	binaryArray = malloc((binaryArrayLen + 1) * sizeof(char));
	
	if(binaryArray == NULL){
		free(inputCopy);
		printf("Error reservando memoria getBinaryString\n");
		return NULL;
	}
	memset(binaryArray, '\0', binaryArrayLen + 1);

	for(i = 0; i < strlen(input); i++){
		if(binaryArrayAux[i] != NULL){
			strcat(binaryArray, binaryArrayAux[i]);
			 free(binaryArrayAux[i]);
		}
	}
	free(inputCopy);
	return binaryArray;
}

char* getOriginalMessage(char *input, HuffmanTable *huffmanTable)
{
	char *original = malloc(sizeof(char));
	original[0] = '\0';
	char *newOriginal = NULL;
	char *symbolRead = NULL;
	char *codeRead = NULL;
	int startCode, endCode;
	int i = 0;

	while (i < strlen(input)){
		startCode = i;
		for(int j = i; j < strlen(input); j++){
			endCode = j;
			codeRead = substring(input, startCode, endCode);
			symbolRead = getSymbol(huffmanTable, codeRead);
			free(codeRead);
			if(symbolRead != NULL){
				newOriginal = concat(original, symbolRead);
				free(original);
				free(symbolRead);
				original = newOriginal;
				break;
			}
		}
		i = endCode + 1;
	}
	return original;
}

