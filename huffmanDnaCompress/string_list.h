#ifndef __STRING_LIST_H
#define __STRING_LIST_H
#define LIST_STRING_BUFFER_LEN 32
typedef struct{
	char** string;
	int capacity;
	int length;
}StringList;

void initStringList(StringList *stringList);
void freeStringList(StringList *stringList);
int addString(StringList *stringList, char *string);
void printStringList(StringList *l);
int containsStringList(StringList *l, char *string);
#endif
