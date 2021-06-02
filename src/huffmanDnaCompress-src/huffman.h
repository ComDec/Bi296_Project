#ifndef __HUFFMAN_H
#define __HUFFMAN_H
#define BINARY_STRING_BUFFER 1024
#include"huffman_table.h"

/**
 * Devuelve un array de punteros a HuffmanTable, que contiene los simbolos con su traduccion.
 * @param frequencies Array de frecuencias.
 * @param symbols Lista de simbolos.
 * @return puntero a HuffmanTable
 */
HuffmanTable* huffman(double *frequencies, char **symbols, int n);

#endif
