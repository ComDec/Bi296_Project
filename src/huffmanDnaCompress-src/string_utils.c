#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_utils.h"

int* searchSubstring(char* input, char* substring, int *nFound, int overlappedSearch)
{
	int substringLength = strlen(substring);
	int inputLength = strlen(input);
	int resComparison;

	int *positionsFound = NULL;
	int positionsFoundLen = 0;
	int i = 0;
	while (i <= inputLength - substringLength){
		resComparison = strncmp(input + i, substring, substringLength);
		if(resComparison == 0){
			if(positionsFoundLen % BUFFER_SEARCH_POSITION_LEN == 0){
				positionsFound = realloc(positionsFound, (positionsFoundLen + BUFFER_SEARCH_POSITION_LEN) * sizeof(int));
			}
			positionsFound[positionsFoundLen] = i;
			positionsFoundLen ++;
			if(overlappedSearch){
				i = i + 1;
			} else {
				i = i + substringLength;
			}
		} else{
			i = i + 1;
		}
	}
	*nFound = positionsFoundLen;
	return positionsFound;
}

void deleteSubstring(char *input, char *substring)
{
	if(input == NULL || substring == NULL){
		return;
	}
	int substringLength = strlen(substring);
	int n = 0;
	int *positions = searchSubstring(input, substring, &n, !OVERLAPPED_SEARCH);

	for(int i = 0; i < n; i++){
		memset(input + positions[i], BLANK_CHAR, substringLength);
	}

	free(positions);
}

int countValidCharacters(char *input)
{
	int count = 0;
	for(int i = 0; i < strlen(input); i++){
		if (input[i] != BLANK_CHAR){
			count ++;
		}
	}

	return count;
}

char* createString(char c)
{
	char *string = malloc(2 * sizeof(char));
	string[0] = c;
	string[1] = '\0';
	return string;
}

char* substring(char* input, int start, int end)
{
	char *substring = NULL;

	if(input == NULL || start < 0 || end >= strlen(input)){
		printf("Error substring. Indice fuera de memoria.\n");
		return NULL;
	}

	int n = end - start + 1;
	substring = malloc((n + 1) * sizeof(char));

	for(int i = 0; i < n; i++){
		substring[i] = input[i + start];
	}
	substring[n] = '\0';

	return substring;
}

char* concat(char *a, char *b)
{
	if(a == NULL || b == NULL){
		return a;
	}
	char *concat = malloc((strlen(a) + strlen(b) + 1) * sizeof(char));
	strncpy(concat, a, strlen(a));
	strncpy(concat + strlen(a), b, strlen(b));
	concat[strlen(a) + strlen(b)] = '\0';
	return concat;
}

double* charFreq(char *input, char *alphabet)
{
	if(input == NULL || alphabet == NULL){
		printf("Error en parametros de charFreq\n");
		return NULL;
	}
	int inputLength = strlen(input);
	int alphabetLength = strlen(alphabet);
	int j;
	double *charFreq = malloc(alphabetLength * sizeof(double));
	for(int i = 0; i < alphabetLength; i++){
		charFreq[i] = 0.0;
	}

	for(int i = 0; i < inputLength; i++){
		j = 0;
		while (j < alphabetLength && alphabet[j] != input[i]){
			j++;
		}
		if (j == alphabetLength){
			printf("Error en charFreq. Caracter de input no encontrado en alphabet.\n");
			free(charFreq);
			return NULL;
		}
		charFreq[j] += 1.0 / (double)inputLength;
	}

	return charFreq;
}

int* charCount(char *input, char *alphabet)
{
	if(input == NULL || alphabet == NULL){
		printf("Error en parametros de charFreq\n");
		return NULL;
	}
	int inputLength = strlen(input);
	int alphabetLength = strlen(alphabet);
	int j;
	int *charCount = malloc(alphabetLength * sizeof(int));
	for(int i = 0; i < alphabetLength; i++){
		charCount[i] = 0;
	}

	for(int i = 0; i < inputLength; i++){
		if(input[i] == BLANK_CHAR){
			continue;
		}

		j = 0;
		while (j < alphabetLength && alphabet[j] != input[i]){
			j++;
		}
		if (j == alphabetLength){
			printf("Error en charCount. Caracter de input no encontrado en alphabet.\n");
			free(charCount);
			return NULL;
		}
		charCount[j] += 1;
	}

	return charCount;
}
