#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_list.h"

void initStringList(StringList *stringList)
{
	if(stringList != NULL){
		stringList->string = malloc(LIST_STRING_BUFFER_LEN * sizeof(char*));
		stringList->capacity = LIST_STRING_BUFFER_LEN;
		stringList->length = 0;
	}
}

void freeStringList(StringList *stringList)
{
	if(stringList != NULL){
		for(int i = 0; i < stringList->length; i++){
			free(stringList->string[i]);
		}
		free(stringList->string);
		free(stringList);
	}
}

int addString(StringList *stringList, char *string)
{
	int newCapacity;
	char **newPointer = NULL;
	if(stringList != NULL && string != NULL){
		/* Si estamos al limite de la capacidad, pedimos mas memoria */
		if(stringList->length == stringList->capacity){
			newCapacity = stringList->capacity + LIST_STRING_BUFFER_LEN;
			newPointer = realloc(stringList->string, newCapacity * sizeof(char*));
			if(newPointer != NULL){
				stringList->string = newPointer;
				stringList->capacity = newCapacity;
			} else {
				return -1;
			}
		}
		stringList->string[stringList->length] = string;
		stringList->length ++;
	}
	return 0;
}

int containsStringList(StringList *l, char *string)
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

void printStringList(StringList *l)
{
	if(l == NULL){
		return;
	}

	for(int i = 0; i < l->length; i++){
		printf("%s\n", l->string[i]);
	}
}
