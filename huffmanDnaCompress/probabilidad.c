#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "probabilidad.h"
double entropiaFuente(double probabilidades [], int n)
{
	double entropia = 0.0;
	int i;
	for (i = 0; i < n; i++){
		entropia += probabilidades[i] * entropiaMensaje(probabilidades[i]);
	}
	return entropia;
}

double entropiaMensaje(double probabilidadMensaje)
{
	return -log2(probabilidadMensaje);
}

double averageCodeLength(HuffmanTable *table)
{
	double averageCodeLength = 0;
	int i;
	if(table != NULL) {
		for(i = 0; i < table->length; i++){
			averageCodeLength += strlen(table->huffmanPair[i]->code) * table->huffmanPair[i]->frequency;
		}
	}

	return averageCodeLength;
}
