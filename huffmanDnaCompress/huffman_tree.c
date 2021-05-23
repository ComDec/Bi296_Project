#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman_tree.h"

void initHuffmanTree(HuffmanTree *huffmanTree)
{
	huffmanTree->root = NULL;
}

void freeHuffmanTree(HuffmanTree *huffmanTree)
{
	postorderHuffmanTree(huffmanTree->root, &freeHuffmanNode);
	free(huffmanTree);
}

void freeHuffmanNode(struct Nodo *node)
{
	free(node->symbol);
	free(node);
}

void addRoot(HuffmanTree *huffmanTree, struct Nodo *root)
{
	huffmanTree->root = root;
}

void addSon(struct Nodo *parent, struct Nodo *son, int position)
{
	if (parent != NULL){
		if (position == LEFT){
			parent->left = son;
		} else if (position == RIGHT){
			parent->right = son;
		}
	}
}

void printNode(struct Nodo *root)
{
	if(root != NULL){
		printf("%s (%.3f)\n", root->symbol, root->frequency);
	}
}

void preorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*))
{
	if(root == NULL){
		return;
	}

	f(root);
	preorderHuffmanTree(root->left, f);
	preorderHuffmanTree(root->right, f);
}

void inorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*))
{
	if(root == NULL){
		return;
	}

	inorderHuffmanTree(root->left, f);
	f(root);
	inorderHuffmanTree(root->right, f);
}

void postorderHuffmanTree(struct Nodo *root, void (*f)(struct Nodo*))
{
	if(root == NULL){
		return;
	}

	postorderHuffmanTree(root->left, f);
	postorderHuffmanTree(root->right, f);
	f(root);
}

void sortHuffmanTrees(HuffmanTree *huffmanTrees[], int n)
{
	HuffmanTree *max = NULL;
	int i, j;

	for(i = 0; i < n - 1; i++){
		max = huffmanTrees[i];
		for(j = i + 1; j < n; j++){
			if (huffmanTrees[j]->root->frequency > max->root->frequency){
				max = huffmanTrees[j];
				huffmanTrees[j] = huffmanTrees[i];
				huffmanTrees[i] = max;
			}
		}
	}
}

HuffmanTree* combineTrees(HuffmanTree *tree1, HuffmanTree *tree2)
{
	HuffmanTree *combinedTree = malloc(sizeof(HuffmanTree));
	initHuffmanTree(combinedTree);

	struct Nodo *root = malloc(sizeof(struct Nodo));
	root->symbol = NULL;
	root->frequency = tree1->root->frequency + tree2->root->frequency;
	
	addSon(root, tree1->root, LEFT);
	addSon(root, tree2->root, RIGHT);

	addRoot(combinedTree, root);

	return combinedTree;
}

HuffmanTree* createHuffmanTree(double *frequencies, char **symbols, int n)
{
	struct Nodo *node;
	HuffmanTree *huffmanTree;
	HuffmanTree *combinedTree;
	HuffmanTree *huffmanTrees[n];
	int i;

	/* Creacion de un arbol por cada simbolo */
	for(i = 0; i < n; i++){
		node = malloc(sizeof(struct Nodo));
		node->symbol = malloc((strlen(symbols[i]) + 1) * sizeof(char));

		node->symbol = strncpy(node->symbol, symbols[i], strlen(symbols[i]));
		node->symbol[strlen(symbols[i])] = '\0';
		node->frequency = frequencies[i];
		node->left = NULL;
		node->right = NULL;

		huffmanTree = malloc(sizeof(HuffmanTree));
		initHuffmanTree(huffmanTree);
		addRoot(huffmanTree, node);

		huffmanTrees[i] = huffmanTree;
	}

	
	/* Combinar arboles hasta que solo quede uno */
	while(n != 1){
		sortHuffmanTrees(huffmanTrees, n);
		combinedTree = combineTrees(huffmanTrees[n - 2], huffmanTrees[n - 1]);

		/* Liberamos la memoria de los arboles combinados. Sus nodos no porque son usados en el combinado
		 * Cuando se libere el arbol combinado final se liberara esta memoria. */
		free(huffmanTrees[n - 1]);
		free(huffmanTrees[n - 2]);
	
		n--;
		huffmanTrees[n - 1] = combinedTree;
	}

	return huffmanTrees[0];
}

int isLeaf(struct Nodo *node)
{
	return node != NULL && node->left == NULL && node->right == NULL;
}


