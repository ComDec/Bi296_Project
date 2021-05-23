#ifndef __PROBABILIDAD_H
#define __PROBABILIDAD_H
#include "huffman_table.h"
/**
 * Calcula la entropia de una fuente.
 * @param probabilidades Array con las probabilidades individuales de cada mensaje.
 * @param n Dimension del array.
 * @return entropia de la fuente.
 */
double entropiaFuente(double probabilidades [], int n);

/**
 * Calcula la entropia de un mensaje.
 * @param probabilidad del mensaje.
 * @return entropia del mensaje.
 */
double entropiaMensaje(double probabilidadMensaje);

/**
 * Calcula la longitud media de un codigo.
 * @param table Tabla de huffman, que contiene cada simbolo junto con su codigo y su frecuencia.
 */
double averageCodeLength(HuffmanTable *table);

#endif
