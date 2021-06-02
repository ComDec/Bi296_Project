#ifndef __HUFFMAN_TABLE_H
#define __HUFFMAN_TABLE_H

#include "huffman_tree.h"

#define BUFFER_HUFFMAN_TABLE_LEN 4
#define HUFFMAN_TABLE_FILE_DELIM " \n"

struct HuffmanPair{
	char *symbol;
	char *code;
	double frequency;
};

/**
 * Estructura que contiene una lista de parejas de Huffman (simbolo y su codigo)
 */
typedef struct{
	struct HuffmanPair **huffmanPair;
	int length;
	int capacity;
}HuffmanTable;

/**
 * Inicializa una tabla de huffman preparada para contener los codigos de n simbolos.
 * @param n El numero de simbolos del alfabeto.
 */
void initHuffmanTable(HuffmanTable *huffmanTable, int n);

/**
 * Libera la memoria usada por una tabla de huffman.
 */
void freeHuffmanTable(HuffmanTable *huffmanTable);

/**
 * Libera la memoria usada por un HuffmanPair
 */
void freeHuffmanPair(struct HuffmanPair *huffmanPair);

/**
 * Devuelve un array de punteros a HuffmanPair, que contiene los simbolos con su traduccion.
 * @param huffmanTree Un arbol de huffman a partir del cual se extraen los simbolos con sus codigos.
 * @param n Numero de simbolos.
 * @return Array de punteros a HuffmanTable
 */
HuffmanTable* createHuffmanTable(HuffmanTree *huffmanTree, int n);

/**
 * Funcion recursiva que forma los codigos de los simbolos.
 * @param root. La raiz de un HuffmanTree.
 * @param table. Una HuffmanTable inicializada y vacia. Se rellenara con los codigos y los simbolos.
 * @param path. Variable donde se van formando los codigos de forma recursiva. En la primera llamada debe ir vacia.
 */
void expandBinaryPaths(struct Nodo *root, HuffmanTable *table, char *path);

/**
 * Inserta el huffmanPair en la huffmanTable.
 * @param huffmanTable.
 * @param huffmanPair.
 * @return 0 si la ejecucion ha sido correcta, -1 si no.
 */
int addHuffmanPair(HuffmanTable *huffmanTable, struct HuffmanPair *huffmanPair);

/**
 * Crea una estructura huffman pair con el simbolo y el codigo especificados por parametro.
 * @symbol
 * @code
 * @return Estructura de tipo HuffmanPair
 */
struct HuffmanPair* createHuffmanPair(char *symbol, char *code, double frequency);

/**
 * exporta a un fichero la tabla con los codigos de huffman.
 * @param huffmanTable La tabla de huffman que exportamos.
 * @param file El fichero donde exportamos la tabla.
 * @return 0 si no hay error, -1 si hay error.
 */
int exportHuffmanTableToFile(HuffmanTable *huffmanTable, char *fileName);

/**
 * Importa una tabla de huffman desde un fichero.
 * @param file El fichero con la informacion que queremos importar.
 * @return HuffmanTable* con la informacion de la tabla de huffman.
 */
HuffmanTable* importHuffmanTableFromFile(char *fileName);

/**
 * Indica si el simbolo pasado por parametro existe en la tabla de huffman
 * @param huffmanTable.
 * @param symbol
 * @return booleano indicando si el simbolo existe en la tabla de huffman.
 */
int existsSymbol(HuffmanTable *huffmanTable, char *symbol);

/**
 * Obtiene el simbolo de un codigo. NULL si el codigo no esta
 */
char* getSymbol(HuffmanTable *table, char *code);

/**
 * Obtiene el simbolo que ocupa mas memoria de huffmanTable
 * @param huffmanTable
 * @return Numero de caracteres del simbolo mas largo.
 */
int getMaxSymbolSize(HuffmanTable *huffmanTable);

/**
 * Ordena las entradas de la tabla acorde al orden que hay en symbols.
 * @param table Tabla de Huffman.
 * @param symbols Array de palabras con los simbolos 
 */
void sortHuffmanTable(HuffmanTable* table, char **symbols);


void printHuffmanPair(struct HuffmanPair *huffmanPair);
void printHuffmanTable(HuffmanTable *table);

#endif
