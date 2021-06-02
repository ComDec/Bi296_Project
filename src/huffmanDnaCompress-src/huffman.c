#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "huffman.h"
#include "huffman_tree.h"
#include "huffman_table.h"
#include "string_list.h"

HuffmanTable* huffman(double *frequencies, char **symbols, int n)
{
	HuffmanTable *table;

	HuffmanTree *huffmanTree = createHuffmanTree(frequencies, symbols, n);

	table = createHuffmanTable(huffmanTree, n);

	/* Ordenamos la tabla para que los simbolos aparezcan en el orden en el que los recibimos en esta funcion 
	 * para que, en el caso de tener simbolos agrupados, no haya duda al codificar el input, al ir sustituyendose
	 * en el orden en el que aparecen en la tabla.*/
	sortHuffmanTable(table, symbols);

	freeHuffmanTree(huffmanTree);

	return table;
}
