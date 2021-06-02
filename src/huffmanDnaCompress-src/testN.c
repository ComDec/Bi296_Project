#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simprob_utils.h"
#include "huffman_table.h"
#include "coder.h"
#include "string_value_list.h"
#include "string_utils.h"
#include "huffman.h"
#include "string_analyzer.h"
#define BITS_PER_SYMBOL 8
char* huffmanSimple(char *input);
char* huffmanComplex(char *input);
void testN();
void testError();

int main(void)
{
	
	testN();
	//testError();
	return 0;
}
void testError()
{
	char *input = "ACTACTTACTACT";

	char *bin = huffmanComplex(input);

	printf("%.3f\n", (double)strlen(input)*BITS_PER_SYMBOL/(double)strlen(bin));

	free(bin);
}

void testN()
{
	double a = 0.50;
	double t = 0.30;
	double c = 0.10;
	double g = 0.10;
	int n = 200;
	int start = 10;
	double rcSimple [n - start];
	double rcComplex [n - start];
	int len [n - start];

	char *seq = NULL;
	char *binSimple = NULL;
	char *binComplex = NULL;
	int size = -1;
	double averageSimpleRC = 0;
	double averageComplexRC = 0;
	for(int i = start; i < n; i++){
		size = i * BITS_PER_SYMBOL;
		seq = createDnaString(a, g, c, t, i);

		binSimple = huffmanSimple(seq);
		binComplex = huffmanComplex(seq);
		
		len[i - start] = i;
		rcSimple[i - start] = (double)size/(double)(strlen(binSimple));
		rcComplex[i - start] = (double)size/(double)(strlen(binComplex));
		printf("%d\t%.3f\t%.3f\n\n", len[i-start], rcSimple[i-start], rcComplex[i-start]);
		averageSimpleRC += rcSimple[i - start];
		averageComplexRC += rcComplex[i - start];
		free(binSimple);
		free(binComplex);
		free(seq);
	}
//	printf("n = [");
//	for(int i = 0; i < n - start; i++){
//		printf("%d ", len[i]);
//	}
//	printf("]\n");
//
//	printf("huffman = [");
//	for(int i = 0; i < n - start; i++){
//		printf("%.2f ", rcSimple[i]);
//	}
//	printf("]\n");

//	printf("algorithm = [");
//	for(int i = 0; i < n - start; i++){
//		printf("%.2f ", rcComplex[i]);
//	}
//	printf("]\n");


	averageSimpleRC = averageSimpleRC / (n - start);
	averageComplexRC = averageComplexRC / (n - start);
	printf("Ratio medio algoritmo simple: %.3f\n", averageSimpleRC);
	printf("Ratio medio algoritmo complejo: %.3f\n", averageComplexRC);
}
char* huffmanSimple(char *input)
{
	char *alphabet[] = {"A", "T", "C", "G"};
	double *symbolFrequency = charFreq(input, "ATCG");
	HuffmanTable *table = huffman(symbolFrequency, alphabet, 4);
	char *bin = getBinaryString(input, table);
	freeHuffmanTable(table);
	free(symbolFrequency);
	return bin;
}

char* huffmanComplex(char *input)
{
	char *alphabet = "ATCG";
	StringValueList *symbols = groupSymbols(input, alphabet);
	HuffmanTable *table = huffman(symbols->value, symbols->string, symbols->length);
	printHuffmanTable(table);
	char *bin = getBinaryString(input, table);
	char* original = getOriginalMessage(bin, table);
	if(strcmp(input, original) != 0){
		printf("Hay errores al comprimir y descomprimir la cadena\n");
	} 
	freeStringValueList(symbols);
	freeHuffmanTable(table);
	free(original);
	return bin;
}
