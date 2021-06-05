#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "string_analyzer.h"
#include "number_utils.h"
#include "string_utils.h"
StringValueList* groupSymbols(char * input, char *alphabet)
{
	if(input == NULL || alphabet == NULL){
		return NULL;
	}

	char *inputCopy = NULL;
	StringValueList* auxExpandPrefix = NULL;
	StringValueList* symbolsGrouped = malloc(sizeof(StringValueList));
	initStringValueList(symbolsGrouped);
	int* symbolCount = NULL;
	char *bestCombination = NULL;
	double bestEvaluation = 0.0;
	double evaluation = 0.0;
	int inputLength = 0;

	
	inputLength = strlen(input);
	inputCopy = malloc((inputLength + 1) * sizeof(char));
	inputCopy = strncpy(inputCopy, input, inputLength);
	inputCopy[inputLength] = '\0';


	do{
		/* Contamos cada simbolo del alfabeto en input. */
		symbolCount = charCount(inputCopy, alphabet);
		bestEvaluation = 0;
		/* Buscamos el simbolo que tiene una evaluacion mayor para seguir buscando un patron a partir de el. */
		for(int i = 0; i < strlen(alphabet); i++){
			evaluation = evaluate(symbolCount[i], strlen(input), 1);
			if (evaluation > bestEvaluation){
				free(bestCombination);
				bestCombination = createString(alphabet[i]);
				bestEvaluation = evaluation;
			} 
		}
		free(symbolCount);

		/* A partir del simbolo mas puntos, se empiezan a buscar subcadenas a partir de el en input */
		//printf("Mejor simbolo encontrado: %s -> %.3f\n", bestCombination, bestEvaluation);
		auxExpandPrefix = malloc(sizeof(StringValueList));
		initStringValueList(auxExpandPrefix);
		expandBestCombination(inputCopy, alphabet, bestCombination, bestEvaluation, auxExpandPrefix);
		/* Insertamos el elemento de mas valor en la lista de simbolos agrupados. */
		int posMaxValue = getPosMaxStringValue(auxExpandPrefix);
		addStringValue(symbolsGrouped, strdup(auxExpandPrefix->string[posMaxValue]), auxExpandPrefix->value[posMaxValue]);
		freeStringValueList(auxExpandPrefix);
		/* Eliminamos de la copia de input las repeticiones de la agrupacion encontrada, sustituyendola por BLANK_CHAR */
		//printf("%s\n", inputCopy);
		deleteSubstring(inputCopy, symbolsGrouped->string[symbolsGrouped->length - 1]);
		
		inputLength = countValidCharacters(inputCopy);
		//printf("%s\n", inputCopy);
	/* Repetimos todo el proceso hasta que se haya agrupado todo el input */
	} while(inputLength != 0);

	free(inputCopy);
	free(bestCombination);
	return symbolsGrouped;
}

void expandBestCombination(char * input, char *alphabet, char *prefix, double prefixValue, StringValueList* expansionList)
{
	int nPositions1 = 0;
	int nPositions2 = 0;
	int nSymbols = strlen(alphabet);
	char * newPrefix1 = NULL;
	char * newPrefix2 = NULL;
	int * positions1 = NULL;
	int * positions2 = NULL;
	double newPrefixValue1 = 0.0;
	double newPrefixValue2 = 0.0;
	int newPrefixLenght = strlen(prefix) + 1;
	addStringValue(expansionList, strdup(prefix), prefixValue);
	for(int i = 0; i < nSymbols; i++){
		newPrefix1 = malloc((newPrefixLenght + 1) * sizeof(char));
		newPrefix1 = strncpy(newPrefix1, prefix, strlen(prefix));
		newPrefix1[newPrefixLenght - 1] = alphabet[i];
		newPrefix1[newPrefixLenght] = '\0';

		newPrefix2 = malloc((newPrefixLenght + 1) * sizeof(char));
		strncpy(newPrefix2 + 1, prefix, strlen(prefix));
		newPrefix2[0] = alphabet[i];
		newPrefix2[newPrefixLenght] = '\0';

		positions1 = searchSubstring(input, newPrefix1, &nPositions1, !OVERLAPPED_SEARCH);
		positions2 = searchSubstring(input, newPrefix2, &nPositions2, !OVERLAPPED_SEARCH);
		free(positions1);
		free(positions2);

		newPrefixValue1 = evaluate(nPositions1, strlen(input), newPrefixLenght);
		newPrefixValue2 = evaluate(nPositions2, strlen(input), newPrefixLenght);
		//printf("\t%s -> %.3f\n", newPrefix1, newPrefixValue1);
		//printf("\t%s -> %.3f\n", newPrefix2, newPrefixValue2);
		if(newPrefixValue1 >= newPrefixValue2 && newPrefixValue1 >= prefixValue * 0.7){
			//printf("Expandiendo: %s con valor: %.3f\n", newPrefix1, newPrefixValue1);
			expandBestCombination(input, alphabet, newPrefix1, newPrefixValue1, expansionList);
		}
		if(newPrefixValue2 >= newPrefixValue1 && newPrefixValue2 >= prefixValue * 0.7){
			//printf("Expandiendo: %s con valor: %.3f\n", newPrefix2, newPrefixValue2);
			expandBestCombination(input, alphabet, newPrefix2, newPrefixValue2, expansionList);
		}

		free(newPrefix1);
		free(newPrefix2);
	}
}

StringList* getAlphabetCombinations(char *alphabet, int combinationLength)
{
	if(alphabet == NULL){
		return NULL;
	}
	char *combination = malloc((combinationLength + 1) * sizeof(char));
	int nSymbols = strlen(alphabet);
	int totalCombinations = pow(nSymbols, combinationLength);
	int *baseChanged;
	StringList* combinations = malloc(sizeof(StringList));
	initStringList(combinations);
	for(int i = 0; i < totalCombinations; i++){
		baseChanged = changeBase(i, nSymbols, combinationLength);
		for(int j = 0; j < combinationLength; j++){
			combination[j] = alphabet[baseChanged[j]];
		}
		combination[combinationLength] = '\0';
		free(baseChanged);
		addString(combinations, strdup(combination));
	}
	free(combination);
	return combinations;
}

double evaluate(int apparitions, int inputLength, int sequenceLength)
{
	double value = ((double)apparitions) / ((double)inputLength) * ((double)sequenceLength);
	return value;
}
