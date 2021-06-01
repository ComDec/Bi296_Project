#ifndef __ENCODE_H
#define __ENCODE_H

#include "huffman_table.h"

#define CODER_BUFFER 64

/**
 * Obtiene una cadena de 1's y 0's resultado de codificar input segun huffmanTable
 * @param input Cadena de entrada.
 * @param huffmanTable Tabla con los simbolos y sus codigos.
 * @return cadena de 1's y 0's resultado de de la codificacion
 */
char* getBinaryString(char *input, HuffmanTable *huffmanTable);

/**
 * Obtiene la cadena original a partir de una cadena de 1's y 0's.
 * @param input Cadena de 1's y 0's.
 * @param huffmanTable Tabla de Huffman con la traduccion.
 * @return El mensaje original.
 */
char* getOriginalMessage(char *input, HuffmanTable *huffmanTable);

#endif
