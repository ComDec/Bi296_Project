#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "huffman_table.h"
void initHuffmanTable(HuffmanTable *huffmanTable, int n)
{
	if(huffmanTable != NULL){
		huffmanTable->huffmanPair = malloc(n * sizeof(struct HuffmanPair*));
		memset(huffmanTable->huffmanPair, '\0', n * sizeof(struct HuffmanPair*));
		huffmanTable->length = 0;
		huffmanTable->capacity = n;
	}
}

void freeHuffmanTable(HuffmanTable *huffmanTable)
{
	if(huffmanTable != NULL){
		for(int i = 0; i < huffmanTable->length; i++){
			freeHuffmanPair(huffmanTable->huffmanPair[i]);
		}
		free(huffmanTable->huffmanPair);
		free(huffmanTable);
	}
}

void freeHuffmanPair(struct HuffmanPair *huffmanPair)
{
	if(huffmanPair != NULL){
		free(huffmanPair->symbol);
		free(huffmanPair->code);
		free(huffmanPair);
	}
}
HuffmanTable* createHuffmanTable(HuffmanTree *huffmanTree, int n)
{
	HuffmanTable *table = malloc(sizeof(HuffmanTable));
	initHuffmanTable(table, n);

	char *res = malloc(128 * sizeof(char));
	memset(res, '\0', 128);

	if(!isLeaf(huffmanTree->root)){
		expandBinaryPaths(huffmanTree->root, table, res);
	} else {
		/* Caso en el que solamente tengamos un simbolo. Se crea la tabla con asignando el codigo 1 a ese simbolo. */
		char *symbol = malloc((strlen(huffmanTree->root->symbol) + 1) * sizeof(char));
		symbol = strncpy(symbol, huffmanTree->root->symbol, strlen(huffmanTree->root->symbol));
		symbol[strlen(huffmanTree->root->symbol)] = '\0';

		char *code = malloc(2 * sizeof(char));
		code[0] = '1';
		code[1] = '\0';

		double frequency = huffmanTree->root->frequency;
		addHuffmanPair(table, createHuffmanPair(symbol, code, frequency));
	}

	free(res);

	return table;
}

void expandBinaryPaths(struct Nodo *root, HuffmanTable *table, char *path)
{
	char *symbol = NULL;
	char *code = NULL;
	double frequency;
	struct HuffmanPair *huffmanPair = NULL;

	if(isLeaf(root)){
		symbol = malloc((strlen(root->symbol) + 1) * sizeof(char));
		symbol = strncpy(symbol, root->symbol, strlen(root->symbol));
		symbol[strlen(root->symbol)] = '\0';

		code = malloc((strlen(path) + 1) * sizeof(char));
		code = strncpy(code, path, strlen(path));
		code[strlen(path)] = '\0';

		frequency = root->frequency;

		huffmanPair = createHuffmanPair(symbol, code, frequency);
		
		addHuffmanPair(table, huffmanPair);
	}
	
	if(root->left != NULL){
		path[strlen(path)] = LEFT + '0';
		expandBinaryPaths(root->left, table, path);
		path[strlen(path)-1] = '\0';
	}
	if(root->right != NULL){
		path[strlen(path)] = RIGHT + '0';
        	expandBinaryPaths(root->right, table, path);
		path[strlen(path)-1] = '\0';
	}
	
}

struct HuffmanPair* createHuffmanPair(char *symbol, char *code, double frequency)
{
	struct HuffmanPair *pair = malloc(sizeof(struct HuffmanPair));
	pair->code = code;
	pair->symbol = symbol;
	pair->frequency = frequency;
	return pair;
}

int addHuffmanPair(HuffmanTable *huffmanTable, struct HuffmanPair *huffmanPair)
{
	int newCapacity;
	struct HuffmanPair **reallocPointer = NULL;

	if(huffmanTable != NULL && huffmanPair != NULL){
		if(huffmanTable->length == huffmanTable->capacity){
			newCapacity = huffmanTable->capacity + BUFFER_HUFFMAN_TABLE_LEN;
			reallocPointer = realloc(huffmanTable->huffmanPair, newCapacity * sizeof(struct HuffmanPair*));
			if (reallocPointer){
				huffmanTable->huffmanPair = reallocPointer;
				huffmanTable->capacity = newCapacity;
			} else{
				return -1;
			}
		}
		huffmanTable->huffmanPair[huffmanTable->length] = huffmanPair;
		huffmanTable->length++;
	}
	return 0;
}

void printHuffmanPair(struct HuffmanPair *huffmanPair)
{
	if(huffmanPair != NULL){
		printf("%s -> %s\t(%.3f)\n", huffmanPair->symbol, huffmanPair->code, huffmanPair->frequency);
	}
}

int exportHuffmanTableToFile(HuffmanTable *huffmanTable, char *fileName)
{
	FILE *file = NULL;
	char stringFreq [80];

	if(fileName != NULL && huffmanTable != NULL){
		file = fopen(fileName, "w");
		if(file != NULL){
			for(int i = 0; i < huffmanTable->length; i++){
				fputs(huffmanTable->huffmanPair[i]->symbol, file);
				fputc(' ', file);
				fputs(huffmanTable->huffmanPair[i]->code, file);
				fputc(' ', file);
				sprintf(stringFreq, "%f", huffmanTable->huffmanPair[i]->frequency);
				fputs(stringFreq, file);
				if(i != huffmanTable->length - 1){
					fputc('\n', file);
				}
			}
			fclose(file);
			return 0;
		}
	}

	
	printf("Error exportando tabla de Huffman a fichero.\n");
	return -1;

	
}


HuffmanTable* importHuffmanTableFromFile(char *fileName)
{
	HuffmanTable *table = NULL;
	struct HuffmanPair *pair = NULL;
	FILE *file = NULL;
	char *code = NULL;
	char *symbol = NULL;
	double frequency;
	char *line = NULL;
	size_t len = 0;
	int read;

	if(fileName != NULL){
		table = malloc(sizeof(HuffmanTable));
		initHuffmanTable(table, 2);

		if((file = fopen(fileName, "r")) != NULL){
			while ((read = getline(&line, &len, file)) != -1) {
				symbol = strtok(line, HUFFMAN_TABLE_FILE_DELIM);
				code = strtok(NULL, HUFFMAN_TABLE_FILE_DELIM);
				frequency = -1.0;
				frequency = atof(strtok(NULL, HUFFMAN_TABLE_FILE_DELIM));
				if(code != NULL && symbol != NULL && frequency >= 0){
					pair = createHuffmanPair(strdup(symbol), strdup(code), frequency);
					addHuffmanPair(table, pair);
				} else{
					printf("Error parseando fichero %s\n", fileName);
					freeHuffmanTable(table);
					free(line);
					fclose(file);
					return NULL;
				}

				
			}
			fclose(file);
			free(line);
		}
	}

	return table;
}

int existsSymbol(HuffmanTable *huffmanTable, char *symbol)
{
	int n;
	int exists = 0;
	if(huffmanTable != NULL && symbol != NULL){
		n = strlen(symbol);
		for (int i = 0; i < huffmanTable->length; i++){
			if(n == strlen(huffmanTable->huffmanPair[i]->symbol) && strncmp(symbol, huffmanTable->huffmanPair[i]->symbol, n)){
				exists = 1;
				break;
			}
		}
	}
	return exists;
}

int getMaxSymbolSize(HuffmanTable *huffmanTable)
{
	int maxSize = -1;
	if(huffmanTable != NULL){
		for(int i = 0; i < huffmanTable->length; i++){
			if(strlen(huffmanTable->huffmanPair[i]->symbol) > maxSize){
				maxSize = strlen(huffmanTable->huffmanPair[i]->symbol);
			}
		}
	}
	return maxSize;
}

void sortHuffmanTable(HuffmanTable* table, char **symbols)
{
	if(table == NULL || symbols == NULL){
		return;
	}

	struct HuffmanPair *huffmanPair = NULL;
	int symbolCount = 0;
	
	while(symbolCount < table->length){
		for(int i = symbolCount; i < table->length; i++){
			if(strcmp(symbols[symbolCount], table->huffmanPair[i]->symbol) == 0){
				huffmanPair = table->huffmanPair[i];
				table->huffmanPair[i] = table->huffmanPair[symbolCount];
				table->huffmanPair[symbolCount] = huffmanPair;
				symbolCount ++;
				break;
			}
		}
	}
	
	
}

char* getSymbol(HuffmanTable *table, char *code)
{
	if(table == NULL || code == NULL){
		return NULL;
	}

	char *symbol = NULL;
	int i = 0;

	while(symbol == NULL && i < table->length){
		if(strcmp(code, table->huffmanPair[i]->code) == 0){
			symbol = strdup(table->huffmanPair[i]->symbol);
		}
		i++;
	}

	return symbol;
}

void printHuffmanTable(HuffmanTable *table)
{
	if(table != NULL){
		for(int i = 0; i < table->length; i++){
			printHuffmanPair(table->huffmanPair[i]);
		}
	}
}
