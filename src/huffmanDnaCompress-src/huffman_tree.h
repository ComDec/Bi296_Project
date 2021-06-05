#ifndef __HUFFMAN_TREE_H
#define __HUFFMAN_TREE_H

#include "string_list.h"

#define LEFT 0
#define RIGHT 1

struct Nodo{
	char *symbol;
	double frequency;
	struct Nodo *left;
	struct Nodo *right;
};

typedef struct{
	struct Nodo *root;
}HuffmanTree;

void initHuffmanTree(HuffmanTree *huffmanTree);

void freeHuffmanTree(HuffmanTree *huffmanTree);

void freeHuffmanNode(struct Nodo *node);

void addRoot(HuffmanTree *huffmanTree, struct Nodo *root);

void addSon(struct Nodo *parent, struct Nodo *son, int position);

void preorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*));

void inorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*));

void postorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*));

void printNode(struct Nodo *root);

int isLeaf(struct Nodo *node);

/**
 * El orden en un arbol huffman viene dado por la frecuencia del nodo raiz. Se ordena de mayor a menor.
 */
void sortHuffmanTrees(HuffmanTree *huffmanTrees[], int n);

/**
 * Combina tree1 y tree2.
 * La etiqueta de la raiz será la suma de las frecuencias de las raíces de los dos árboles que se unen, y cada uno de estos árboles será un hijo del nuevo arbol.
 * Tambien se etiquetan las dos ramas del nuevo arbol: con un 0 la de la izquierda, y con un 1 la de la derecha.
 * @return Un puntero al huffmanTree resultado de combinar tree1 y tree2.
 */
HuffmanTree* combineTrees(HuffmanTree *tree1, HuffmanTree *tree2);

/**
 * Devuelve el arbol de Huffman a partir del cual se puede construir el codigo del alfabeto.
 * @param frequencies Array de frecuencias.
 * @param symbols Array de simbolos.
 * @param n Dimension de los arrays.
 * @return HuffmanTree
 */
HuffmanTree* createHuffmanTree(double *frequencies, char **symbols, int n);

#endif
