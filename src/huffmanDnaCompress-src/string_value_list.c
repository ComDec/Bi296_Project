#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_value_list.h"

void initStringValueList(StringValueList *l)
{
	if(l != NULL){
		l->string = malloc(LIST_STRING_VALUE_BUFFER_LEN * sizeof(char*));
		l->value = malloc(LIST_STRING_VALUE_BUFFER_LEN * sizeof(double));
		l->capacity = LIST_STRING_VALUE_BUFFER_LEN;
		l->length = 0;
	}
}

void freeStringValueList(StringValueList *l)
{
	if(l != NULL){
		for(int i = 0; i < l->length; i++){
			free(l->string[i]);
		}
		free(l->string);
		free(l->value);
		free(l);
	}
}

int addStringValue(StringValueList *l, char *string, double value){
	int newCapacity;
	char **newStringPointer = NULL;
	double *newValuePointer = NULL;
	if(l != NULL && string != NULL){
		/* Si estamos al limite de la capacidad, pedimos mas memoria */
		if(l->length == l->capacity){
			newCapacity = l->capacity + LIST_STRING_VALUE_BUFFER_LEN;
			newStringPointer = realloc(l->string, newCapacity * sizeof(char*));
			newValuePointer = realloc(l->value, newCapacity * sizeof(double));
			if(newStringPointer != NULL && newValuePointer != NULL){
				l->string = newStringPointer;
				l->value = newValuePointer;
				l->capacity = newCapacity;
			} else {
				return -1;
			}
		}
		l->string[l->length] = string;
		l->value[l->length] = value;
		l->length ++;
	}
	return 0;
}

void printStringValueList(StringValueList *l)
{
	if(l != NULL){
		for(int i = 0; i < l->length; i++){
			printf("%s -> %.3f\n", l->string[i], l->value[i]);
		}
	}
}
int containsStringValueList(StringValueList *l, char *string)
{
	if(l == NULL || string == NULL){
		return 0;
	}

	for(int i = 0; i < l->length; i++){
		if(strcmp(l->string[i], string) == 0){
			return 1;
		}
	}
	return 0;
}

int getPosMaxStringValue(StringValueList *l)
{
	if(l == NULL){
		return NULL;
	}

	double maxValue = -1;
	int maxPos = -1;
	int maxLength = -1;
	for(int i = 0; i < l->length; i++){
		if((l->value[i] > maxValue) || (l->value[i] == maxValue && strlen(l->string[i]) > maxLength)){
			maxPos = i;
			maxValue = l->value[i];
			maxLength = strlen(l->string[i]);
		}
	}

	return maxPos;
}
