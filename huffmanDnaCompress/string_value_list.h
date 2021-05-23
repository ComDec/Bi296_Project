#ifndef __STRING_VALUE_LIST_H
#define __STRING_VALUE_LIST_H
#define LIST_STRING_VALUE_BUFFER_LEN 32
typedef struct{
	char** string;
	double* value;
	int capacity;
	int length;
}StringValueList;

void initStringValueList(StringValueList *l);
void freeStringValueList(StringValueList *l);
int addStringValue(StringValueList *l, char *string, double value);
void printStringValueList(StringValueList *l);
int containsStringValueList(StringValueList *l, char *string);
int getPosMaxStringValue(StringValueList *l);
#endif
